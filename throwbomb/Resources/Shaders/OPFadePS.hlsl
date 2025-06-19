// OPFadePS.hlsl
#include "Fade.hlsli"
#define MAX_EXPLOSIONS 5



cbuffer ExplosionBuffer : register(b1)
{
    float4 explosionPositions[MAX_EXPLOSIONS];
};



Texture2D tex : register(t0);
SamplerState samLinear : register(s0);
extern float4x4 g_worldViewProjection;

//time.ｙに爆発状態をキープを入れます。　０が爆発してる、１が爆発していない
float4 main(PS_INPUT input) : SV_TARGET
{
    //float4 texColor = tex.Sample(samLinear, input.Tex);
    //float alpha = 1.0f;

    //for (int i = 0; i < MAX_EXPLOSIONS; i++)
    //{
    //    // zに爆発フラグ（0 = 爆発済み）
    //    if (explosionPositions[i].z >= 1.0f)
    //    {
    //        continue;
    //    }
        
    //    float2 centerUV = explosionPositions[i].xy;
        
    //    float dist = distance(input.Tex.xy, centerUV);
        
    //    // 半径：最後だけカスタム、それ以外は0.5
    //    float radius = (i == MAX_EXPLOSIONS - 1) ? 10.0f : 0.5f; 
        
    //    float fade = smoothstep(0.0f, radius, dist);
        
        
    //    alpha *= fade ;
    //    texColor.a *= alpha ;
        
        
    //}

    //return texColor;
    
    
    
    float4 texColor = tex.Sample(samLinear, input.Tex);
    float alpha = 1.0f;

    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        // 無効な爆発は無視
        if (explosionPositions[i].z >= 1.0f){
            continue;
        }

        float2 centerUV = explosionPositions[i].xy;

        // 徐々に広がる半径（時間に比例） ※速度調整可
        float baseRadius = (i == MAX_EXPLOSIONS - 1) ? 15.0f : 0.5f;
        float radius = saturate( time.x/ 3.0f) * baseRadius;
        // ↑ 2.0f は広がるスピードの係数（時間が2秒で最大半径に達する）

        float dist = distance(input.Tex.xy, centerUV);
        float fade = smoothstep(0.0f, radius, dist);

        alpha *= fade;
        
    }

    texColor.a *= alpha;
    return texColor;
}
