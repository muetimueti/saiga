/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/opengl/indexedVertexBuffer.h"
#include "saiga/opengl/shader/basic_shaders.h"
#include "saiga/opengl/texture/CubeTexture.h"
#include "saiga/opengl/texture/Texture.h"
#include "saiga/opengl/vertex.h"

namespace Saiga
{
/**
 * Renders a texture into the given screen-space viewport.
 * The texture will be stretched into the given size.
 */
class SAIGA_OPENGL_API TextureDisplay
{
   public:
    TextureDisplay();
    void render(TextureBase* texture, const ivec2& position, const ivec2& size);

   private:
    std::shared_ptr<MVPTextureShader> shader;
    IndexedVertexBuffer<VertexNT, GLuint> buffer;
};

}  // namespace Saiga
