﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "saiga/config.h"
#ifdef SAIGA_USE_SDL

#    include "saiga/opengl/imgui/imgui_impl_sdl_gl3.h"

#    include "sdl_window.h"

namespace Saiga
{
SDLWindow::SDLWindow(WindowParameters windowParameters, OpenGLParameters openglParameter)
    : OpenGLWindow(windowParameters, openglParameter)
{
    create();
}

SDLWindow::~SDLWindow()
{
    destroy();
}


bool SDLWindow::initWindow()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);


    if (windowParameters.fullscreen())
    {
        windowParameters.width  = current.w;
        windowParameters.height = current.h;
    }

    //    OpenGLParameters& oparams = windowParameters.openglparameters;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, openglParameters.versionMajor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, openglParameters.versionMinor);

    switch (openglParameters.profile)
    {
        case OpenGLParameters::Profile::ANY:
            // that is the default value
            break;
        case OpenGLParameters::Profile::CORE:
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            break;
        case OpenGLParameters::Profile::COMPATIBILITY:
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
            break;
    }

    if (openglParameters.debug) SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    if (openglParameters.forwardCompatible)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);


    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);

    /*  \param flags The flags for the window, a mask of any of the following:
     *               ::SDL_WINDOW_FULLSCREEN,    ::SDL_WINDOW_OPENGL,
     *               ::SDL_WINDOW_HIDDEN,        ::SDL_WINDOW_BORDERLESS,
     *               ::SDL_WINDOW_RESIZABLE,     ::SDL_WINDOW_MAXIMIZED,
     *               ::SDL_WINDOW_MINIMIZED,     ::SDL_WINDOW_INPUT_GRABBED,
     *               ::SDL_WINDOW_ALLOW_HIGHDPI.
     */
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

    if (windowParameters.resizeAble) flags |= SDL_WINDOW_RESIZABLE;
    if (windowParameters.borderLess()) flags |= SDL_WINDOW_BORDERLESS;
    if (windowParameters.fullscreen()) flags |= SDL_WINDOW_FULLSCREEN;
    if (windowParameters.hidden) flags |= SDL_WINDOW_HIDDEN;

    // Create window
    window = SDL_CreateWindow(getName().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, getWidth(),
                              getHeight(), flags);
    if (window == NULL)
    {
        std::cout << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create context
    gContext = SDL_GL_CreateContext(window);
    if (gContext == NULL)
    {
        std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Use Vsync
    if (SDL_GL_SetSwapInterval(windowParameters.vsync ? 1 : 0) < 0)
    {
        std::cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << std::endl;
    }


    return success;
}

bool SDLWindow::initInput()
{
    // Enable text input
    SDL_StartTextInput();


    return true;
}

std::shared_ptr<ImGui_GL_Renderer> SDLWindow::createImGui()
{
    if (windowParameters.imguiParameters.enable)
    {
        return std::make_shared<ImGui_SDL_Renderer>(window, windowParameters.imguiParameters);
    }
    return {};
}

bool SDLWindow::shouldClose()
{
    return SDL_EventHandler::shouldQuit() || !running;
}

void SDLWindow::checkEvents()
{
    SDL_EventHandler::update();
}

void SDLWindow::swapBuffers()
{
    SDL_GL_SwapWindow(window);
}


void SDLWindow::freeContext()
{
    //    SDL_EventHandler::reset();
    // Disable text input
    SDL_StopTextInput();

    // Destroy window
    SDL_DestroyWindow(window);
    window = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}

void SDLWindow::loadGLFunctions()
{
    glbinding::GetProcAddress gpa = [](const char* pa) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(pa); };
    initOpenGL(gpa);
}

bool SDLWindow::resizeWindow(Uint32 windowId, int width, int height)
{
    if (SDL_GetWindowID(window) == windowId)
    {
        this->resize(width, height);
    }
    return false;
}

}  // namespace Saiga

#endif
