/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#ifndef COLORIZE_H
#define COLORIZE_H

#include "hsv.h"
#include "shaderConfig.h"

/**
 * Creates a smooth HSV color transition from Blue(=0) to Red(=1).
 * The returned value is in RGB space.
 */
FUNC_DECL vec3 colorizeBlueRed(float alpha)
{
    alpha = 1.f - saturate(alpha);
    return hsv2rgb(vec3(alpha * (240.0f / 360.0f), 1, 1));
}

/**
 * Similar to above but from Red(=0) to Green(=1).
 */
FUNC_DECL vec3 colorizeRedGreen(float alpha)
{
    alpha = saturate(alpha);
    return hsv2rgb(vec3(alpha * (120.0f / 360.0f), 1, 1));
}

/**
 * A temperature like color sheme from 0=black to 1=white
 */
FUNC_DECL vec3 colorizeFusion(float x)
{
    float t = saturate(x);
    return saturate(vec3(sqrt(t), t * t * t, max(sin(3.1415 * 1.75 * t), pow(t, 12.0))));
}

FUNC_DECL vec3 colorizeFusionHDR(float x)
{
    float t = saturate(x);
    return colorizeFusion(sqrt(t)) * (t * 2.0f + 0.5f);
}

/**
 * All credits to:
 * Viridis approximation, Jerome Liard, August 2016
 * https://www.shadertoy.com/view/XtGGzG
 */
FUNC_DECL vec3 colorizeInferno(float x)
{
    x       = saturate(x);
    vec4 x1 = vec4(1.0, x, x * x, x * x * x);  // 1 x x2 x3
    vec4 x2 = x1 * x1.w * x;                   // x4 x5 x6 x7
    return vec3(dot(vec4(x1.xyzw), vec4(-0.027780558, +1.228188385, +0.278906882, +3.892783760)) +
                    dot(vec2(x2.xy), vec2(-8.490712758, +4.069046086)),
                dot(vec4(x1.xyzw), vec4(+0.014065206, +0.015360518, +1.605395918, -4.821108251)) +
                    dot(vec2(x2.xy), vec2(+8.389314011, -4.193858954)),
                dot(vec4(x1.xyzw), vec4(-0.019628385, +3.122510347, -5.893222355, +2.798380308)) +
                    dot(vec2(x2.xy), vec2(-3.608884658, +4.324996022)));
}

FUNC_DECL vec3 colorizeMagma(float x)
{
    x       = saturate(x);
    vec4 x1 = vec4(1.0, x, x * x, x * x * x);  // 1 x x2 x3
    vec4 x2 = x1 * x1.w * x;                   // x4 x5 x6 x7
    return vec3(dot(vec4(x1.xyzw), vec4(-0.023226960, +1.087154378, -0.109964741, +6.333665763)) +
                    dot(vec2(x2.xy), vec2(-11.640596589, +5.337625354)),
                dot(vec4(x1.xyzw), vec4(+0.010680993, +0.176613780, +1.638227448, -6.743522237)) +
                    dot(vec2(x2.xy), vec2(+11.426396979, -5.523236379)),
                dot(vec4(x1.xyzw), vec4(-0.008260782, +2.244286052, +3.005587601, -24.279769818)) +
                    dot(vec2(x2.xy), vec2(+32.484310068, -12.688259703)));
}

FUNC_DECL vec3 colorizePlasma(float x)
{
    x       = saturate(x);
    vec4 x1 = vec4(1.0, x, x * x, x * x * x);  // 1 x x2 x3
    vec4 x2 = x1 * x1.w * x;                   // x4 x5 x6 x7
    return vec3(dot(x1, vec4(+0.063861086, +1.992659096, -1.023901152, -0.490832805)) +
                    dot(vec2(x2.xy), vec2(+1.308442123, -0.914547012)),
                dot(x1, vec4(+0.049718590, -0.791144343, +2.892305078, +0.811726816)) +
                    dot(vec2(x2.xy), vec2(-4.686502417, +2.717794514)),
                dot(x1, vec4(+0.513275779, +1.580255060, -5.164414457, +4.559573646)) +
                    dot(vec2(x2.xy), vec2(-1.916810682, +0.570638854)));
}

#endif
