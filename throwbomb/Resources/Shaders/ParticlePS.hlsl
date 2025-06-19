
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
    // 時間（30秒以内の場合のみ色変更）
    float currentTime = time.x;

    // 30秒以内かどうかをstep関数で判定
    float mask = step(currentTime, 30.0f); 

    // 赤色の強さをsin波で変更
    float redIntensity = 0.5f * (sin(currentTime*0.5f) + 1.0f);

    // テクスチャのサンプリング
    float4 color = tex.Sample(samLinear, input.Tex);

    // 赤色成分を更新
    color.r = redIntensity * mask; // redIntensityを0～1の範囲で変動させる

    return color;
}