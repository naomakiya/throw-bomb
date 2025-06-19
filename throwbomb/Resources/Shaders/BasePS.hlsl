/*
	ピクセルシェーダ
*/
#include"Common.hlsli"
struct PS_Input
{
    float4 PoaitionWS : TEXCOORD0;
    float3 NormalWS   : TEXCOORD1;
    float4 Diffuse    : COLOR0;
};

float4 main(PS_Input input) : SV_Target0
{

    // テクスチャから色をサンプリング
    float4 textureColor = Texture.Sample(Sampler, input.PoaitionWS.xy);

    // 最終的な色を計算（テクスチャの色と拡散反射を掛け合わせる）
    float3 finalColor =  textureColor.rgb;

    // 透明度を0.5に設定（アルファ値）
    return float4(finalColor, 0.5f);
   
}