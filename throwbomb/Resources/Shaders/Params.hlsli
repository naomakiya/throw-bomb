/*
	@file	Params.hlsli
	@brief	各種パラメータ用
*/

// 定数バッファ：各種パラメータ用
cbuffer Parameters : register(b1)
{
    float3 lightDirection   : packoffset(c0);
    float4x4 viewMatrix     : packoffset(c1);
    float3 cameraPosition   : packoffset(c5);

}

// 頂点シェーダ入力用
struct VS_Input
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

// ピクセルシェーダ入力用
struct PS_Input
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float DistToEye : TEXCOORD1; // 視点との距離
};
