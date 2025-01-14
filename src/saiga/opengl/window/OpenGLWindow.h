﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/opengl/imgui/imgui_opengl.h"
#include "saiga/opengl/opengl.h"
#include "saiga/core/window/WindowBase.h"

namespace Saiga
{
class Camera;
class Image;

class SAIGA_OPENGL_API OpenGLWindow : public WindowBase
{
   public:
    OpenGLWindow(WindowParameters windowParameters, OpenGLParameters openglParameters);
    virtual ~OpenGLWindow();

    bool create();
    void destroy();

    void renderImGui(bool* p_open = nullptr);


    // reading the default framebuffer
    void readToExistingImage(Image& out);
    void readToImage(Image& out);

    // read the default framebuffer and save to file
    void screenshot(const std::string& file);
    void screenshotRender(const std::string& file);
    void screenshotRenderDepth(const std::string& file);
    void getDepthFloat(Image& out);



    virtual std::shared_ptr<ImGui_GL_Renderer> createImGui() { return nullptr; }

    virtual void swap();
    virtual void update(float dt);

    OpenGLParameters openglParameters;

   protected:
    virtual bool initWindow() = 0;
    virtual bool initInput()  = 0;
    virtual bool shouldClose() { return !running; }
    virtual void checkEvents()     = 0;
    virtual void swapBuffers()     = 0;
    virtual void freeContext()     = 0;
    virtual void loadGLFunctions() = 0;

    void sleep(tick_t ticks);
};

}  // namespace Saiga
