
#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float distortion(float2 center, float r, float d_r, float2 uv, float aspect)
{
    float2 uv2 = center;
    uv2.x *= aspect;

    uv.x *= aspect;

    float dist = distance(uv2, uv);

    return 1 - smoothstep(r, d_r, dist);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // ���ԁi30�b�ȓ��̏ꍇ�̂ݐF�ύX�j
    float currentTime = time.x;

    // 30�b�ȓ����ǂ�����step�֐��Ŕ���
    float mask = step(currentTime, 30.0f); 

    // �ԐF�̋�����sin�g�ŕύX
    float redIntensity = 0.5f * (sin(currentTime*0.5f) + 1.0f);

    // �e�N�X�`���̃T���v�����O
    float4 color = tex.Sample(samLinear, input.Tex);

    // �ԐF�������X�V
    color.r = redIntensity * mask; // redIntensity��0�`1�͈̔͂ŕϓ�������

    return color;
}