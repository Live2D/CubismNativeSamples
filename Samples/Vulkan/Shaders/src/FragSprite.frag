/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#version 460
#extension GL_GOOGLE_include_directive : enable
layout(binding = 0) uniform UBO
{
    vec4 u_baseColor;
}ubo;

layout(binding = 1) uniform sampler2D s_texture0;
layout(binding = 2) uniform sampler2D s_texture1;

layout(location = 0) in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 color = texture(s_texture0, v_texCoord);
    outColor = vec4(color.rgb * color.a, color.a) * ubo.u_baseColor;
}
