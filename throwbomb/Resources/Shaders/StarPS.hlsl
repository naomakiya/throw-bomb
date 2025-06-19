#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	// �摜����F���T���v�����O
    float4 output = tex.Sample(samLinear, input.Tex);

// �摜�̔����������o���A�����ɂ���
    if (output.r == 1.0f && output.g == 1.0f && output.b == 1.0f)
    {
        output.a = 0.0f; // ���F�����𓧖���
    }

    return output * input.Color;
}