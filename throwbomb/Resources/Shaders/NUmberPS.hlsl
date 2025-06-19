
#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

static int digitCount = 3;

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
    
    float2 uv = input.Tex;

    // テクスチャの幅と高さ（桁を表示するテクスチャ分割数）
    float w = 5.0f; // 横方向の分割数
    float h = 2.0f; // 縦方向の分割数

    // 時間を整数化
    float m_time = floor(time.x);

    // 必要な桁数を計算（例: 123 -> 3桁）
    int m_count = max(1, floor(log10(m_time + 1)) + 1);

    // 初期カラーを透明に設定
    float4 color = float4(0, 0, 0, 0);
 
    // ２桁を表示
    for (int i = 0; i < digitCount; i++)
    {
        // 1桁目（i == 0）をスキップ
        if (i == 0)
            continue;
        // 現在の桁の数字を計算（1の位、10の位、100の位…）
        float digit = floor((m_time / pow(10, i)) % 10);

        // 各桁のUVを調整
        float2 digitUV = uv;

        // 桁ごとの位置を計算（右から順に配置）
        digitUV.x = uv.x * w - (4 - i); // 右端から左へ
        if (digitUV.x < 0.0 || digitUV.x >= 1.0)
            continue; // 表示範囲外はスキップ

        digitUV.x /= w; // 横方向をテクスチャ分割数に合わせる
        digitUV.y /= h; // 縦方向をテクスチャ分割数に合わせる

        // 現在の桁の数字に基づき、テクスチャのUVオフセットを計算
        digitUV.x += (1.0 / w) * (digit % w);
        digitUV.y += (1.0 / h) * floor(digit / w);
        
       // float4 digitColor = tex.Sample(samLinear, digitUV);
        float4 digitColor = tex.SampleLevel(samLinear, digitUV, 0);
        color = digitColor;

    }

    return color;
}