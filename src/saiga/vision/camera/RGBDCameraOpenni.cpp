/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "RGBDCameraOpenni.h"

#ifdef SAIGA_USE_OPENNI2
#    include "saiga/core/image/imageTransformations.h"
#    include "saiga/core/imgui/imgui.h"
#    include "saiga/core/util/Thread/threadName.h"

#    include "internal/noGraphicsAPI.h"

#    include <OpenNI.h>
#    include <thread>

namespace Saiga
{
#    define CHECK_NI(_X)                                                                     \
        {                                                                                    \
            auto ret = _X;                                                                   \
            if (ret != openni::STATUS_OK)                                                    \
            {                                                                                \
                std::cout << "Openni error in " << #_X << std::endl                          \
                          << "code: " << ret << std::endl                                    \
                          << "message: " << openni::OpenNI::getExtendedError() << std::endl; \
                SAIGA_ASSERT(0);                                                             \
            }                                                                                \
        }



RGBDCameraOpenni::RGBDCameraOpenni(const RGBDIntrinsics& intr)
    : RGBDCamera(intr),
      //      frameBuffer(10),
      depthFactor(1.0 / intr.depthFactor)
{
    CHECK_NI(openni::OpenNI::initialize());

    while (!tryOpen())
    {
        std::cout << "RGBD Camera Open Failed. Trying again shortly." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

RGBDCameraOpenni::~RGBDCameraOpenni()
{
    std::cout << "~RGBDCameraOpenni" << std::endl;
}


bool RGBDCameraOpenni::getImageSync(RGBDFrameData& data)
{
    makeFrameData(data);
    while (!waitFrame(data, true))
    {
        if (!foundCamera) return false;
    }
    return true;
}

bool RGBDCameraOpenni::getImage(RGBDFrameData& data)
{
    makeFrameData(data);

    bool ret = waitFrame(data, false);

    if (ret)
    {
        return true;
    }
    return false;
}

void RGBDCameraOpenni::close()
{
    resetCamera();

    std::cout << "RGBDCameraInput closed." << std::endl;
}

bool RGBDCameraOpenni::isOpened()
{
    return foundCamera;
}

void RGBDCameraOpenni::updateCameraSettings()
{
    updateS = true;
}

void RGBDCameraOpenni::imgui()
{
    if (ImGui::Checkbox("autoexposure", &autoexposure)) updateS = true;
    if (ImGui::Checkbox("autoWhiteBalance", &autoWhiteBalance)) updateS = true;
}

bool RGBDCameraOpenni::tryOpen()
{
    resetCamera();

    openni::Status rc = openni::STATUS_OK;


    const char* deviceURI = openni::ANY_DEVICE;



    rc = device->open(deviceURI);
    if (rc != openni::STATUS_OK)
    {
        std::cout << "device open failed: " << rc << std::endl;
        return false;
    }

    CHECK_NI(depth->create(*device, openni::SENSOR_DEPTH));
    CHECK_NI(color->create(*device, openni::SENSOR_COLOR));
    SAIGA_ASSERT(depth->isValid() && color->isValid());

    auto fps = intrinsics().fps;
    {
        auto co                                       = intrinsics().deptho;
        const openni::Array<openni::VideoMode>& modes = depth->getSensorInfo().getSupportedVideoModes();
        int found                                     = -1;
        for (int i = 0; i < modes.getSize(); ++i)
        {
            const openni::VideoMode& mode = modes[i];

            if (mode.getResolutionX() == co.w && mode.getResolutionY() == co.h && mode.getFps() == fps &&
                mode.getPixelFormat() == openni::PIXEL_FORMAT_DEPTH_1_MM)
            {
                found = i;
                break;
            }
        }
        SAIGA_ASSERT(found != -1);
        CHECK_NI(depth->setVideoMode(modes[found]));
    }

    {
        auto co                                       = intrinsics().rgbo;
        const openni::Array<openni::VideoMode>& modes = color->getSensorInfo().getSupportedVideoModes();
        int found                                     = -1;
        for (int i = 0; i < modes.getSize(); ++i)
        {
            const openni::VideoMode& mode = modes[i];

            if (mode.getResolutionX() == co.w && mode.getResolutionY() == co.h && mode.getFps() == fps &&
                mode.getPixelFormat() == openni::PIXEL_FORMAT_RGB888)
            {
                found = i;
                break;
            }
        }
        SAIGA_ASSERT(found != -1);
        CHECK_NI(color->setVideoMode(modes[found]));
    }


    CHECK_NI(color->start());
    CHECK_NI(depth->start());


    auto settings    = color->getCameraSettings();
    autoexposure     = settings->getAutoExposureEnabled();
    autoWhiteBalance = settings->getAutoWhiteBalanceEnabled();

    foundCamera = true;

    std::cout << "RGBD Camera opened." << std::endl;

    return true;
}

void RGBDCameraOpenni::resetCamera()
{
    m_depthFrame = std::make_shared<openni::VideoFrameRef>();
    m_colorFrame = std::make_shared<openni::VideoFrameRef>();

    depth = std::make_shared<openni::VideoStream>();
    color = std::make_shared<openni::VideoStream>();

    device = std::make_shared<openni::Device>();
}

bool RGBDCameraOpenni::waitFrame(RGBDFrameData& data, bool wait)
{
    openni::VideoStream* streams[2] = {depth.get(), color.get()};
    int streamIndex;

    int timeout = wait ? 1000 : 0;

    if (!device->isValid() || !depth->isValid() || !color->isValid())
    {
        std::cout << "wait frame on invalid device!" << std::endl;
        foundCamera = false;
    }

    auto wret = openni::OpenNI::waitForAnyStream(streams, 2, &streamIndex, timeout);
    if (wret != openni::STATUS_OK)
    {
        return false;
    }

    setNextFrame(data);


    bool ret = true;
    if (streamIndex == 0)
    {
        ret &= readDepth(data.depthImg);

        wret = openni::OpenNI::waitForAnyStream(streams + 1, 1, &streamIndex, timeout);
        if (wret != openni::STATUS_OK) return false;

        ret &= readColor(data.colorImg);
    }
    else
    {
        ret &= readColor(data.colorImg);

        wret = openni::OpenNI::waitForAnyStream(streams, 1, &streamIndex, timeout);
        if (wret != openni::STATUS_OK) return false;

        ret &= readDepth(data.depthImg);
    }


    return ret;
}

bool RGBDCameraOpenni::readDepth(DepthImageType::ViewType depthImg)
{
    auto res = depth->readFrame(m_depthFrame.get());
    if (res != openni::STATUS_OK) return false;

    SAIGA_ASSERT(m_depthFrame->getWidth() == depthImg.w && m_depthFrame->getHeight() == depthImg.h);

    ImageView<uint16_t> rawDepthImg(m_depthFrame->getHeight(), m_depthFrame->getWidth(),
                                    m_depthFrame->getStrideInBytes(), (void*)m_depthFrame->getData());

    for (int i = 0; i < rawDepthImg.height; ++i)
    {
        for (int j = 0; j < rawDepthImg.width; ++j)
        {
            depthImg(i, j) = rawDepthImg(i, rawDepthImg.width - j - 1) * depthFactor;
        }
    }

    return true;
}

bool RGBDCameraOpenni::readColor(RGBImageType::ViewType colorImg)
{
    auto res = color->readFrame(m_colorFrame.get());
    if (res != openni::STATUS_OK) return false;

    SAIGA_ASSERT(m_colorFrame->getWidth() == colorImg.w && m_colorFrame->getHeight() == colorImg.h);

    ImageView<ucvec3> rawImg(m_colorFrame->getHeight(), m_colorFrame->getWidth(), m_colorFrame->getStrideInBytes(),
                             (void*)m_colorFrame->getData());

    for (int i = 0; i < rawImg.height; ++i)
    {
        for (int j = 0; j < rawImg.width; ++j)
        {
            colorImg(i, j) = make_ucvec4(rawImg(i, rawImg.width - j - 1), 255);
        }
    }

    return true;
}

void RGBDCameraOpenni::updateSettingsIntern()
{
    SAIGA_ASSERT(foundCamera);
    auto settings = color->getCameraSettings();

    (settings->setAutoExposureEnabled(autoexposure));
    (settings->setAutoWhiteBalanceEnabled(autoWhiteBalance));


    updateS = false;
}



}  // namespace Saiga
#endif
