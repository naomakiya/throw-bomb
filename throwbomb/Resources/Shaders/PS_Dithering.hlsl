/*
	@file	PS_Dithering.hlsl
	@brief	�f�B�U�����O
*/
#include "Common.hlsli"
#include "Params.hlsli"

// �f�B�U�p�^�[��(Bayer Matrix)�̒�`
static const int pattern[4][4] =
{
    {  0, 32,  8, 40 },
    { 48, 16, 56, 24 },
    { 12, 44,  4, 36 },
    { 60, 28, 52, 20 },
};


float4 main(PS_Input input) : SV_Target0
{
    // �@���𐳋K������
    float3 normal = normalize(input.Normal);
    
    // ----------------------------------------------------------- //
    // �f�B�U�p�^�[�����v�Z����
    // ----------------------------------------------------------- //
    
    //���̃s�N�Z����X���W�AY���W���S�Ŋ������]����v�Z����
    int x = (int) fmod(input.Position.x, 4.0f);
    int y = (int) fmod(input.Position.y, 4.0f);
    
    //��ł��Ƃ߂�X��Y�𗘗p���āA���̃s�N�Z���̃e�U�����O���擾����
    int dither = pattern[y][x];
    
    //���S�ɃN���b�v�����͈͂��`����
    float clipRange = 2.5f;
    
    //���_�ƃN���b�v�͈͂܂ł��`����
    float eyeToClipRange = max(0.0f, input.DistToEye - clipRange);
    
    //�N���b�v�����v�Z����
    float clipRate = 1.0f - min(1.0f, eyeToClipRange / 19.0f);
    
    //clipRate�𗘗p���ăs�N�Z���L�����s��
    clip(dither - 64 * clipRate);
    
    // ----------------------------------------------------------- //
    // �f�B�t���[�Y
    // ----------------------------------------------------------- //

    //���˃x�N�g���̋t�x�N�g���𐳋K�����遨���C�g����������Ɍ������x�N�g��
    float3 toLight = normalize(-lightDirection + input.Position.xyz);
    
    //�@���x�N�g���ƃ��C�g�x�N�g�����甽�ˌ��̋������v�Z����i�O�|�P�j
    float intensity1 = max(dot(normal, toLight), 1.0f);
    
    //�g�U���˂̋������v�Z����
    float3 diffuse = DiffuseColor.rgb * LightDiffuseColor[0] * intensity1 + EmissiveColor;
    
    // ----------------------------------------------------------- //
    // �X�y�L����
    // ----------------------------------------------------------- //

    //�Ώۍ��W����J�������W�Ɍ������x�N�g���𐳋K������
    float3 toEye = normalize(cameraPosition - input.Position.xyz);
    
    //�n�[�t�x�N�g�����v�Z���� : ���C�g�x�N�g���{���_�x�N�g��
    float3 halfVector = normalize(toLight + toEye);
    
    //�@���x�N�g���ƃn�[�t�x�N�g�����甽�ˌ��̋������v�Z����(0-1)
    float intensity2 = max(dot(normal, halfVector), 0.1f);
    
    //���ʔ��ˌ��̋������v�Z����
    float3 specular = pow(intensity2, SpecularPower) * LightSpecularColor[0];

    // ----------------------------------------------------------- //
    // �ŏI�J���[
    // ----------------------------------------------------------- //
    
    //�e�N�X�`���[�J���[���T���v������
    float4 textureColor = Texture.Sample(Sampler, input.TexCoord);
    
    //�e�N�X�`���[�̐F�Ɋg�U���˂Ƌ��ʔ��˂𔽉f����
    float3 finalColor = textureColor.rgb * diffuse ;
    
    return float4(finalColor,1.0f);

}
