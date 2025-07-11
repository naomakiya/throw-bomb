
#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

static int digitCount = 7;

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

    float digitWidth = 1.0 / digitCount;

    float4 color = float4(0, 0, 0, 0);

    for (int i = 0; i < digitCount; i++)
    {
        // 各桁の横範囲（左→右）
        float xMin = digitWidth * i;
        float xMax = xMin + digitWidth;

        // このピクセルが表示する範囲外ならスキップ
        if (uv.x < xMin || uv.x >= xMax)
            continue;

        // 右から左に数字を並べる（i=0が左端、digitIndex=6→i=6が1の位）
        int digitIndex = digitCount - 1 - i;

        float digit = floor((m_time / pow(10, digitIndex)) % 10);

        // 桁内でのUVをローカルにスケーリング
        float2 digitUV;
        digitUV.x = (uv.x - xMin) / digitWidth;
        digitUV.y = uv.y;

        // テクスチャ内UVに変換
        digitUV.x /= w;
        digitUV.y /= h;
        digitUV.x += (1.0 / w) * (digit % w);
        digitUV.y += (1.0 / h) * floor(digit / w);

        float4 digitColor = tex.SampleLevel(samLinear, digitUV, 0);
        color = digitColor;
    }


    return color;
}