/**
 * Copyright (c) 2017 Darius Rückert 
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */

##GL_VERTEX_SHADER

#version 330
layout(location=0) in vec3 in_position;


out vec2 tc;


void main() {
    tc = vec2(in_position.x,in_position.y);
    tc = tc * 0.5f + 0.5f;
    gl_Position = vec4(in_position.x,in_position.y,0,1);
}

##GL_FRAGMENT_SHADER
#version 330
in vec2 tc;
uniform sampler2D image;

layout(location=0) out vec4 out_color;

void main() {
    out_color = texture( image, tc );
}


