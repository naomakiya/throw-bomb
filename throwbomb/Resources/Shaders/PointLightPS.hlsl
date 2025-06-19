/*
	@file	PointLightPS.hlsl
	@brief	�|�C���g���C�g
*/
#include "Common.hlsli"


// ���C�g�̍\����
cbuffer PointLight : register(b1)
{
    float4 Position;
    float4 Color;
    float4 time;
};


// �s�N�Z���V�F�[�_���͗p
struct PS_Input
{
    float2 TexCoord : TEXCOORD0;
    float4 PositionWS : TEXCOORD1;
    float3 NormalWS : TEXCOORD2;
    float4 Diffuse : COLOR0;
};

// ���̌����p�����[�^
static const float att0 = 0.05f; // ��{������������������
static const float att1 = 0.01f; // ���`������ǉ�
static const float att2 = 0.02f; // �񎟌������ɘa

// ���C���֐�
float4 main(PS_Input input) : SV_Target0
{
    // �e�N�X�`���J���[���T���v������
    float4 textureColor = Texture.Sample(Sampler, input.TexCoord) * input.Diffuse;
    
    // �ŏI�J���[
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);

    // ���[���h�ϊ����ꂽ�@���𐳋K������
    float3 normal = normalize(input.NormalWS);
    
    // ���C�g�̐F��萔�o�b�t�@����擾
    float3 dynamicColor = float3(1.0f, 0.84f, 0.0f); // ���F���
    // ----------------------------------------------------------- //
    // Lambert�g�U����
    // ----------------------------------------------------------- //
    
    // ���C�g�̈ʒu��������̕������v�Z
    float3 lightDirection = input.PositionWS.xyz - Position.xyz;
    
    // ���C�g�x�N�g���F���C�g���W�֌������P�ʃx�N�g���i���˃x�N�g���̋t�x�N�g���j
    float3 toLight = normalize(-lightDirection);
    
    // ���C�g�܂ł̋������v�Z����
    float lightDistance = length(lightDirection);
    
    // �@���x�N�g���ƃ��C�g�x�N�g�����甽�ˌ��̋������v�Z����(0-1)
    float intensity1 = max(dot(normal, toLight), 0.0f);
    // �g�U���˂̋������v�Z����
    float3 diffuse = DiffuseColor.rgb * Color.rgb * intensity1 + EmissiveColor;
    
    // ----------------------------------------------------------- //
    // Phong���ʔ���
    // ----------------------------------------------------------- //
        
     // ���_�x�N�g���F�Ώۍ��W����J�������W�Ɍ������x�N�g���𐳋K������
    float3 toEye = normalize(EyePosition - input.PositionWS.xyz);
    
     // �n�[�t�x�N�g�����v�Z����F���C�g�x�N�g���{���_�x�N�g��
    float3 halfVector = normalize(toLight + toEye);

     // �@���x�N�g���ƃn�[�t�x�N�g�����甽�ˌ��̋������v�Z����(0-1)
    float intensity2 = max(dot(normal, halfVector), 0.5f);

     // ���ʔ��ˌ��̋������v�Z����
    float3 specular = pow(intensity2, 64) * SpecularColor * 4.0f;

     
     // ----------------------------------------------------------- //
     // ���̌���
     // ----------------------------------------------------------- //

     // ���C�g�̋����ɂ����̌������v�Z����
     // 1
    float attenuation = 1.0f / (att0 + att1 * lightDistance + att2 * lightDistance * lightDistance);
    
     // diffuse�̌������l������
    diffuse *= attenuation;

     // specular�̌������l������
    specular *= attenuation;

     
     // ----------------------------------------------------------- //
     // �ŏI�J���[
     // ----------------------------------------------------------- //
     
     // �e�N�X�`���̐F�Ɋg�U���˂Ƌ��ʔ��˂𔽉f����
    finalColor += textureColor.rgb * diffuse + specular;
        
    
    
    return float4(finalColor, 1.0f);
}
