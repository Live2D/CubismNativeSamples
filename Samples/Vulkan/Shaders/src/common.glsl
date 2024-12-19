/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

layout(binding = 0) uniform UBO
{
    mat4 u_matrix;
    mat4 u_clipMatrix;
    vec4 u_baseColor;
    vec4 u_channelFlag;
}ubo;

layout(binding = 1) uniform sampler2D s_texture0;
layout(binding = 2) uniform sampler2D s_texture1;
