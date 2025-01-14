﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

#pragma once

#include "saiga/core/math/math.h"

#include "imageView.h"


namespace Saiga
{
namespace ImageDraw
{
// Draw a filled circle
template <typename T, typename S>
void drawCircle(ImageView<T> img, const vec2& position, float radius, const S& color)
{
    auto r2 = radius * radius;
    for (int dy = -radius; dy <= radius; ++dy)
    {
        for (int dx = -radius; dx <= radius; ++dx)
        {
            float distance = vec2(dx, dy).squaredNorm();
            if (distance > r2) continue;
            int px = position[0] + dx;
            int py = position[1] + dy;
            img.clampedWrite(py, px, color);
        }
    }
}


// draw a (1 pixel wide) line
template <typename T, typename S>
void drawLineBresenham(ImageView<T> img, const vec2& start, const vec2& end, const S& color)
{
    // Source
    // https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
    int x0 = round(start[0]);
    int y0 = round(start[1]);
    int x1 = round(end[0]);
    int y1 = round(end[1]);

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        img.clampedWrite(y0, x0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

// draw the outline of a circle
template <typename T, typename S>
void drawCircleBresenham(ImageView<T> img, const vec2& position, int radius, const S& color)
{
    // source https://www.thecrazyprogrammer.com/2016/12/bresenhams-midpoint-circle-algorithm-c-c.html
    int x   = radius;
    int y   = 0;
    int err = 0;

    while (x >= y)
    {
        // symmetry!
        img.clampedWrite(position.y() + y, position.x() + x, color);
        img.clampedWrite(position.y() + x, position.x() + y, color);
        img.clampedWrite(position.y() + x, position.x() - y, color);
        img.clampedWrite(position.y() + y, position.x() - x, color);
        img.clampedWrite(position.y() - y, position.x() - x, color);
        img.clampedWrite(position.y() - x, position.x() - y, color);
        img.clampedWrite(position.y() - x, position.x() + y, color);
        img.clampedWrite(position.y() - y, position.x() + x, color);

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }

        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}


}  // namespace ImageDraw
}  // namespace Saiga
