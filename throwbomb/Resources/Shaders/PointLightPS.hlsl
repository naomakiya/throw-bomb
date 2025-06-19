/*
	@file	PointLightPS.hlsl
	@brief	ポイントライト
*/
#include "Common.hlsli"


// ライトの構造体
cbuffer PointLight : register(b1)
{
    float4 Position;
    float4 Color;
    float4 time;
};


// ピクセルシェーダ入力用
struct PS_Input
{
    float2 TexCoord : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS : TEXCOORD2;
    float4 Diffuse : COLOR0;
};

// 光の減衰パラメータ
static const float att0 = 0.05f; // 基本減衰率を小さくする
static const float att1 = 0.01f; // 線形減衰を追加
static const float att2 = 0.02f; // 二次減衰を緩和

// メイン関数
float4 main(PS_Input input) : SV_Target0
{
    // テクスチャカラーをサンプルする
    float4 textureColor = Texture.Sample(Sampler, input.TexCoord) * input.Diffuse;
    
    // 最終カラー
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);

    // ワールド変換された法線を正規化する
    float3 normal = normalize(input.NormalWS);
    
    // ライトの色を定数バッファから取得
    float3 dynamicColor = float3(1.0f, 0.84f, 0.0f); // 金色を基調
    // ----------------------------------------------------------- //
    // Lambert拡散反射
    // ----------------------------------------------------------- //
    
    // ライトの位置から光源の方向を計算
    float3 lightDirection = input.PositionWS.xyz - Position.xyz;
    
    // ライトベクトル：ライト座標へ向かう単位ベクトル（入射ベクトルの逆ベクトル）
    float3 toLight = normalize(-lightDirection);
    
    // ライトまでの距離を計算する
    float lightDistance = length(lightDirection);
    
    // 法線ベクトルとライトベクトルから反射光の強さを計算する(0-1)
    float intensity1 = max(dot(normal, toLight), 0.0f);
    // 拡散反射の強さを計算する
    float3 diffuse = DiffuseColor.rgb * Color.rgb * intensity1 + EmissiveColor;
    
    // ----------------------------------------------------------- //
    // Phong鏡面反射
    // ----------------------------------------------------------- //
        
     // 視点ベクトル：対象座標からカメラ座標に向かうベクトルを正規化する
    float3 toEye = normalize(EyePosition - input.PositionWS.xyz);
    
     // ハーフベクトルを計算する：ライトベクトル＋視点ベクトル
    float3 halfVector = normalize(toLight + toEye);

     // 法線ベクトルとハーフベクトルから反射光の強さを計算する(0-1)
    float intensity2 = max(dot(normal, halfVector), 0.5f);

     // 鏡面反射光の強さを計算する
    float3 specular = pow(intensity2, 64) * SpecularColor * 4.0f;

     
     // ----------------------------------------------------------- //
     // 光の減衰
     // ----------------------------------------------------------- //

     // ライトの距離による光の減衰を計算する
     // 1
    float attenuation = 1.0f / (att0 + att1 * lightDistance + att2 * lightDistance * lightDistance);
    
     // diffuseの減衰を考慮する
    diffuse *= attenuation;

     // specularの減衰を考慮する
    specular *= attenuation;

     
     // ----------------------------------------------------------- //
     // 最終カラー
     // ----------------------------------------------------------- //
     
     // テクスチャの色に拡散反射と鏡面反射を反映する
    finalColor += textureColor.rgb * diffuse + specular;
        
    
    
    return float4(finalColor, 1.0f);
}
