/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#version 120

attribute vec3 position;
attribute vec2 uv;
varying vec2 vuv;

void main(void)
{
    gl_Position = vec4(position, 1.0);
    vuv = uv;
}
