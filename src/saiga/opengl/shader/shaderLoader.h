﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/core/util/DataStructures/ObjectCache.h"
#include "saiga/core/util/assert.h"
#include "saiga/core/util/fileChecker.h"
#include "saiga/core/util/singleton.h"
#include "saiga/opengl/shader/shader.h"
#include "saiga/opengl/shader/shaderPartLoader.h"

#include <iostream>
namespace Saiga
{
class SAIGA_OPENGL_API ShaderLoader
{
    ObjectCache<std::string, std::shared_ptr<Shader>, ShaderPart::ShaderCodeInjections> cache;

   public:
    virtual ~ShaderLoader() {}
    template <typename shader_t>
    std::shared_ptr<shader_t> load(const std::string& name,
                                   const ShaderPart::ShaderCodeInjections& sci = ShaderPart::ShaderCodeInjections());

    void reload();
    bool reload(std::shared_ptr<Shader> shader, const std::string& name, const ShaderPart::ShaderCodeInjections& sci);

    void clear() { cache.clear(); }
};


template <typename shader_t>
std::shared_ptr<shader_t> ShaderLoader::load(const std::string& name, const ShaderPart::ShaderCodeInjections& sci)
{
    std::string fullName = SearchPathes::shader(name);
    if (fullName.empty())
    {
        std::cout << "Could not find file '" << name << "'. Make sure it exists and the search pathes are set."
                  << std::endl;
        std::cerr << SearchPathes::shader << std::endl;
        SAIGA_ASSERT(0);
    }


    std::shared_ptr<Shader> objectBase;
    auto inCache = cache.get(fullName, objectBase, sci);


    std::shared_ptr<shader_t> object;
    if (inCache)
    {
        object = std::dynamic_pointer_cast<shader_t>(objectBase);
    }
    else
    {
        ShaderPartLoader spl(fullName, sci);
        if (spl.load())
        {
            object = spl.createShader<shader_t>();
        }

        cache.put(fullName, object, sci);
    }
    SAIGA_ASSERT(object);

    return object;
}


// Let's make it a global variable to simplify the code a lot
// Note: we need the export here to ensure it is included in the
// symbol table of libsaiga.so
inline SAIGA_OPENGL_API ShaderLoader shaderLoader;


}  // namespace Saiga
