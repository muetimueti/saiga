/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#include "glImageFormat.h"



namespace Saiga
{
static const GLenum ImageElementTypeGL[] = {GL_BYTE,         GL_UNSIGNED_BYTE, GL_SHORT,  GL_UNSIGNED_SHORT, GL_INT,
                                            GL_UNSIGNED_INT, GL_FLOAT,         GL_DOUBLE, GL_INVALID_ENUM};


static const GLenum ImageTypeInternalGL[] = {
    GL_R8,   GL_RG8,   GL_RGB8,   GL_RGBA8,   GL_R8,           GL_RG8,          GL_RGB8,         GL_RGBA8,

    GL_R16,  GL_RG16,  GL_RGB16,  GL_RGBA16,  GL_R16,          GL_RG16,         GL_RGB16,        GL_RGBA16,

    GL_R32I, GL_RG32I, GL_RGB32I, GL_RGBA32I, GL_R32UI,        GL_RG32UI,       GL_RGB32UI,      GL_RGBA32UI,

    GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F, GL_INVALID_ENUM, GL_INVALID_ENUM, GL_INVALID_ENUM, GL_INVALID_ENUM};



GLenum getGlInternalFormat(ImageType type, bool srgb, bool integral)
{
    GLenum t = ImageTypeInternalGL[type];
    if (srgb)
    {
        // currently there are only 2 srgb formats.
        if (t == GL_RGB8) t = GL_SRGB8;
        if (t == GL_RGBA8) t = GL_SRGB8_ALPHA8;
    }
    return t;
}

GLenum getGlFormat(ImageType type)
{
    static const GLenum formats[] = {GL_RED, GL_RG, GL_RGB, GL_RGBA};
    return formats[channels(type) - 1];
}

GLenum getGlType(ImageType type)
{
    return ImageElementTypeGL[elementType(type)];
}

}  // namespace Saiga
