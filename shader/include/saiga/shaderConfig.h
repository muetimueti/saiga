/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#ifndef SHADER_CONFIG_H
#define SHADER_CONFIG_H


#if defined(GL_core_profile)
#define SHADER_DEVICE
#else
#define SHADER_HOST
#endif


#ifdef SHADER_HOST
#define FUNC_DECL inline
#define GLM_FORCE_SWIZZLE
#include "saiga/util/glm.h"
using namespace glm;
#else
#define FUNC_DECL
#endif

/**
 * This is actually a usefull function, so declare it here instead of in
 * hlslDefines.h
 */
FUNC_DECL float saturate(float x) { return clamp( x, 0.0f, 1.0f ); }
FUNC_DECL vec2  saturate(vec2 x)  { return clamp( x, vec2(0.0f), vec2(1.0f) ); }
FUNC_DECL vec3  saturate(vec3 x)  { return clamp( x, vec3(0.0f), vec3(1.0f) ); }
FUNC_DECL vec4  saturate(vec4 x)  { return clamp( x, vec4(0.0f), vec4(1.0f) ); }

#endif