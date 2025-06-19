/*
	�s�N�Z���V�F�[�_
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

    // �e�N�X�`������F���T���v�����O
    float4 textureColor = Texture.Sample(Sampler, input.PoaitionWS.xy);

    // �ŏI�I�ȐF���v�Z�i�e�N�X�`���̐F�Ɗg�U���˂��|�����킹��j
    float3 finalColor =  textureColor.rgb;

    // �����x��0.5�ɐݒ�i�A���t�@�l�j
    return float4(finalColor, 0.5f);
   
}