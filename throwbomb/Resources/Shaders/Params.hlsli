/*
	@file	Params.hlsli
	@brief	�e��p�����[�^�p
*/

// �萔�o�b�t�@�F�e��p�����[�^�p
cbuffer Parameters : register(b1)
{
    float3 lightDirection   : packoffset(c0);
    float4x4 viewMatrix     : packoffset(c1);
    float3 cameraPosition   : packoffset(c5);

}

// ���_�V�F�[�_���͗p
struct VS_Input
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
};

// �s�N�Z���V�F�[�_���͗p
struct PS_Input
{
    float4 Position : SV_Position;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float DistToEye : TEXCOORD1; // ���_�Ƃ̋���
};
