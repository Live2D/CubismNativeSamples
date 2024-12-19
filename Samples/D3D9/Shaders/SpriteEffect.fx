/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

float4x4 projectMatrix;
float4 baseColor;
texture mainTexture;

// サンプリング
sampler mainSampler = sampler_state{
    texture = <mainTexture>;
};

// 頂点シェーダーの入力定義
struct VS_IN {
    float2 pos : POSITION;
    float2 uv : TEXCOORD0;
};

// 頂点シェーダーの出力定義
struct VS_OUT {
    float4 Position : POSITION0;
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
float4 PixelNormal(VS_OUT In) : COLOR0{
    float4 color = tex2D(mainSampler, In.uv) * baseColor;
    return color;
}

// テクニックとパス
technique ShaderNames_Normal {
    pass p0{
        VertexShader = compile vs_2_0 VertNormal();
        PixelShader = compile ps_2_0 PixelNormal();
    }
}
