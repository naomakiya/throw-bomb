/*
	@file	PS_Dithering.hlsl
	@brief	ディザリング
*/
#include "Common.hlsli"
#include "Params.hlsli"

// ディザパターン(Bayer Matrix)の定義
static const int pattern[4][4] =
{
    {  0, 32,  8, 40 },
    { 48, 16, 56, 24 },
    { 12, 44,  4, 36 },
    { 60, 28, 52, 20 },
};


float4 main(PS_Input input) : SV_Target0
{
    // 法線を正規化する
    float3 normal = normalize(input.Normal);
    
    // ----------------------------------------------------------- //
    // ディザパターンを計算する
    // ----------------------------------------------------------- //
    
    //このピクセルのX座標、Y座標を４で割った余りを計算する
    int x = (int) fmod(input.Position.x, 4.0f);
    int y = (int) fmod(input.Position.y, 4.0f);
    
    //上でもとめたXとYを利用して、このピクセルのテザリングを取得する
    int dither = pattern[y][x];
    
    //完全にクリップされる範囲を定義する
    float clipRange = 2.5f;
    
    //視点とクリップ範囲までを定義する
    float eyeToClipRange = max(0.0f, input.DistToEye - clipRange);
    
    //クリップ率を計算する
    float clipRate = 1.0f - min(1.0f, eyeToClipRange / 19.0f);
    
    //clipRateを利用してピクセルキルを行う
    clip(dither - 64 * clipRate);
    
    // ----------------------------------------------------------- //
    // ディフューズ
    // ----------------------------------------------------------- //

    //入射ベクトルの逆ベクトルを正規化する→ライトがある方向に向かうベクトル
    float3 toLight = normalize(-lightDirection + input.Position.xyz);
    
    //法線ベクトルとライトベクトルから反射光の強さを計算する（０－１）
    float intensity1 = max(dot(normal, toLight), 1.0f);
    
    //拡散反射の強さを計算する
    float3 diffuse = DiffuseColor.rgb * LightDiffuseColor[0] * intensity1 + EmissiveColor;
    
    // ----------------------------------------------------------- //
    // スペキュラ
    // ----------------------------------------------------------- //

    //対象座標からカメラ座標に向かうベクトルを正規化する
    float3 toEye = normalize(cameraPosition - input.Position.xyz);
    
    //ハーフベクトルを計算する : ライトベクトル＋視点ベクトル
    float3 halfVector = normalize(toLight + toEye);
    
    //法線ベクトルとハーフベクトルから反射光の強さを計算する(0-1)
    float intensity2 = max(dot(normal, halfVector), 0.1f);
    
    //鏡面反射光の強さを計算する
    float3 specular = pow(intensity2, SpecularPower) * LightSpecularColor[0];

    // ----------------------------------------------------------- //
    // 最終カラー
    // ----------------------------------------------------------- //
    
    //テクスチャーカラーをサンプルする
    float4 textureColor = Texture.Sample(Sampler, input.TexCoord);
    
    //テクスチャーの色に拡散反射と鏡面反射を反映する
    float3 finalColor = textureColor.rgb * diffuse ;
    
    return float4(finalColor,1.0f);

}
