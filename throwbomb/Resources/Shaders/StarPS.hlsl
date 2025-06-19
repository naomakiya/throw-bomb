#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	// 画像から色をサンプリング
    float4 output = tex.Sample(samLinear, input.Tex);

// 画像の白部分を検出し、透明にする
    if (output.r == 1.0f && output.g == 1.0f && output.b == 1.0f)
    {
        output.a = 0.0f; // 白色部分を透明に
    }

    return output * input.Color;
}