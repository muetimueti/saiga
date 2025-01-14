/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/opengl/texture/TextureBase.h"

namespace Saiga
{
class SAIGA_OPENGL_API Texture2D : public TextureBase
{
   public:
    Texture2D() : TextureBase(GL_TEXTURE_2D) {}

    void setDefaultParameters();
    bool fromImage(const Image& img, bool srgb, bool flipY);
    void updateFromImage(const Image& img);

    int getWidth() { return width; }
    int getHeight() { return height; }


    void create(int width, int height, GLenum color_type, GLenum internal_format, GLenum data_type,
                const void* data = nullptr);


    /**
     * Uploads 2-D image data.
     * Wrapper for: glTexImage2D
     */
    void upload(const void* data);

    void resize(int width, int height);
    void uploadSubImage(int x, int y, int width, int height, void* data);

   private:
    int width, height;
};

using Texture = Texture2D;

class SAIGA_OPENGL_API MultisampledTexture2D : public TextureBase
{
   public:
    int samples = 4;
    int width, height;

    MultisampledTexture2D(int samples) : TextureBase(GL_TEXTURE_2D_MULTISAMPLE), samples(samples) {}
    virtual ~MultisampledTexture2D() {}

    void setDefaultParameters();
    void uploadData(const void* data);
};

}  // namespace Saiga
