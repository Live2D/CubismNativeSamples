/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#version 100

precision mediump float;

varying vec2 vuv;
uniform sampler2D texture;
uniform vec4 baseColor;

void main(void)
{
    gl_FragColor = texture2D(texture, vuv) * baseColor;
}
