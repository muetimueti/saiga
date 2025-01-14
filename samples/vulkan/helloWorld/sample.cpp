﻿/*
 * Vulkan Example - imGui (https://github.com/ocornut/imgui)
 *
 * Copyright (C) 2017 by Sascha Willems - www.saschawillems.de
 *
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "sample.h"

#include "saiga/core/image/imageTransformations.h"
#include "saiga/core/math/random.h"
#include "saiga/core/util/color.h"

#include <saiga/core/imgui/imgui.h>

#if defined(SAIGA_OPENGL_INCLUDED)
#    error OpenGL was included somewhere.
#endif

VulkanExample::VulkanExample()
{
    //    init(renderer->base());
    auto& base = renderer->base();


    {
        auto tex = std::make_shared<Saiga::Vulkan::Texture2D>();
        Saiga::Image img("box.png");
        if (img.type == Saiga::UC3)
        {
            std::cout << "adding alplha channel" << std::endl;
            Saiga::TemplatedImage<ucvec4> img2(img.height, img.width);
            std::cout << img << " " << img2 << std::endl;
            Saiga::ImageTransformation::addAlphaChannel(img.getImageView<ucvec3>(), img2.getImageView(), 255);
            tex->fromImage(base, img2);
        }
        else
        {
            std::cout << img << std::endl;
            tex->fromImage(base, img);
        }
        texture = tex;
    }


    assetRenderer.init(base, renderer->renderPass);
    lineAssetRenderer.init(base, renderer->renderPass, 2);
    pointCloudRenderer.init(base, renderer->renderPass, 5);
    texturedAssetRenderer.init(base, renderer->renderPass);
    textureDisplay.init(base, renderer->renderPass);

    textureDes = textureDisplay.createAndUpdateDescriptorSet(*texture);


    box.loadObj("box.obj");

    //    box.loadObj("cat.obj");
    box.init(renderer->base());
    box.descriptor = texturedAssetRenderer.createAndUpdateDescriptorSet(*box.textures[0]);

    teapot.loadObj("teapot.obj");
    teapot.computePerVertexNormal();
    teapot.init(renderer->base());
    teapotTrans.setScale(vec3(2, 2, 2));
    teapotTrans.translateGlobal(vec3(0, 2, 0));
    teapotTrans.calculateModel();

    plane.createCheckerBoard(ivec2(20, 20), 1.0f, Saiga::Colors::firebrick, Saiga::Colors::gray);
    plane.init(renderer->base());

    grid.createGrid(10, 10);
    grid.init(renderer->base());

    frustum.createFrustum(camera.proj, 2, make_vec4(1), true);
    frustum.init(renderer->base());

    pointCloud.init(base, 1000 * 1000);
    for (int i = 0; i < 1000 * 1000; ++i)
    {
        Saiga::VertexNC v;
        v.position               = make_vec4(linearRand(make_vec3(-3), make_vec3(3)), 1);
        v.color                  = make_vec4(linearRand(make_vec3(0), make_vec3(1)), 1);
        pointCloud.pointCloud[i] = v;
    }
}

VulkanExample::~VulkanExample()
{
    VLOG(3) << "~VulkanExample";
}

void VulkanExample::update(float dt)
{
    if (!ImGui::captureKeyboard())
    {
        camera.update(dt);
    }
    if (!ImGui::captureMouse())
    {
        camera.interpolate(dt, 0);
    }


    if (change)
    {
        for (auto& v : pointCloud.pointCloud)
        {
            v.position = make_vec4(linearRand(make_vec3(-3), make_vec3(3)), 1);
            v.color    = make_vec4(linearRand(make_vec3(0), make_vec3(1)), 1);
        }
        change        = false;
        uploadChanges = true;
    }
}

void VulkanExample::transfer(vk::CommandBuffer cmd)
{
    assetRenderer.updateUniformBuffers(cmd, camera.view, camera.proj);
    lineAssetRenderer.updateUniformBuffers(cmd, camera.view, camera.proj);
    pointCloudRenderer.updateUniformBuffers(cmd, camera.view, camera.proj);
    texturedAssetRenderer.updateUniformBuffers(cmd, camera.view, camera.proj);

    if (uploadChanges)
    {
        pointCloud.updateBuffer(cmd, 0, pointCloud.capacity);
        uploadChanges = false;
    }
}


void VulkanExample::render(vk::CommandBuffer cmd)
{
    if (displayModels)
    {
        if (assetRenderer.bind(cmd))
        {
            assetRenderer.pushModel(cmd, identityMat4());
            plane.render(cmd);

            assetRenderer.pushModel(cmd, teapotTrans.model);
            teapot.render(cmd);
        }

        if (lineAssetRenderer.bind(cmd))
        {
            lineAssetRenderer.pushModel(cmd, translate(vec3(-5, 1.5f, 0)));
            teapot.render(cmd);

            auto gridMatrix = rotate(0.5f * pi<float>(), vec3(1, 0, 0));
            gridMatrix      = translate(gridMatrix, vec3(0, -10, 0));
            lineAssetRenderer.pushModel(cmd, gridMatrix);
            grid.render(cmd);
        }


        if (pointCloudRenderer.bind(cmd))
        {
            pointCloudRenderer.pushModel(cmd, translate(vec3(10, 2.5f, 0)));
            pointCloud.render(cmd, 0, pointCloud.capacity);
        }

        if (texturedAssetRenderer.bind(cmd))
        {
            texturedAssetRenderer.pushModel(cmd, translate(vec3(-10, 1, 0)));
            texturedAssetRenderer.bindTexture(cmd, box.descriptor);
            box.render(cmd);
        }
    }



    if (textureDisplay.bind(cmd))
    {
        textureDisplay.renderTexture(cmd, textureDes, vec2(10, 10), vec2(100, 50));
    }
}

void VulkanExample::renderGUI()
{
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Example settings");
    ImGui::Checkbox("Render models", &displayModels);



    if (ImGui::Button("change point cloud"))
    {
        change = true;
    }


    if (ImGui::Button("reload shader"))
    {
        //        texturedAssetRenderer.shaderPipeline.reload();
        texturedAssetRenderer.reload();
        assetRenderer.reload();
    }


    ImGui::End();
    //    return;

    window->renderImGui();
    //    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    //    ImGui::ShowTestWindow();
}
int main(const int argc, const char* argv[])
{
    using namespace Saiga;

    {
        VulkanExample example;

        example.run();
    }

    return 0;
}
