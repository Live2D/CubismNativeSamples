/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

// 定数バッファ
cbuffer ConstantBuffer {
    float4x4 projectMatrix;
    float4x4 clipMatrix;
    float4 baseColor;
    float4 channelFlag;
}

// テクスチャとサンプラー設定
Texture2D mainTexture : register(t0);
SamplerState mainSampler : register(s0);

// 頂点シェーダーの入力定義
struct VS_IN {
    float2 pos : POSITION0;
    float2 uv : TEXCOORD0;
};

// 頂点シェーダーの出力定義
struct VS_OUT {
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 clipPosition : TEXCOORD1;
};

// 頂点シェーダー
VS_OUT VertNormal(VS_IN In) {
    VS_OUT Out = (VS_OUT)0;
    Out.Position = mul(float4(In.pos, 0.0f, 1.0f), projectMatrix);
    Out.uv.x = In.uv.x;
    Out.uv.y = 1.0 - +In.uv.y;
    return Out;
}

// ピクセルシェーダー
float4 PixelNormal(VS_OUT In) : SV_Target {
    float4 color = mainTexture.Sample(mainSampler, In.uv) * baseColor;
    return color;
}
