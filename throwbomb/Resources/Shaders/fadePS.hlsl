#include "fade.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    
     // ���ԂɊ�Â��Ďg�p����e�N�X�`����؂�ւ� (time.x��30�b�ȉ����ǂ����őI��)
    float useTex2 = step(1.0, time.y); // 1�Ȃ�tex2��I��

    // �e�N�X�`���̑I���i
    float4 output = lerp(tex.Sample(samLinear, input.Tex), tex2.Sample(samLinear, input.Tex), useTex2);
	//	�摜�\��
    //float4 output = tex.Sample(samLinear, input.Tex);

    // �A�X�y�N�g��̒��� (��ʂ�1280x720�̏ꍇ)
    float aspect = 1280.0f / 720.0f;
    float2 uv = input.Tex;
    uv.x *= aspect; // �������ɃA�X�y�N�g��𒲐�

    // ��ʂ̒����̍��W���`
    float2 center = float2(0.5f * aspect, 0.5f);

    // ���݂̃s�N�Z���ʒu�Ɖ�ʒ��S�Ƃ̋������v�Z
    float dist = distance(uv, center);

    // ���ԂɊ�Â��������O�T�C�Y�̓��I�ω� (sin�֐��Ŏ��Ԃ̐i�s�ɏ]���ĕω�)
    float timeFactor = sin(time.x / 1.0f); // 0����1�܂ł̒l���J��Ԃ�
    float radius = lerp(0.0f, 1.25f, timeFactor); // 0.0����1.25�܂Ń����O�̔��a��ω�

    // �t�F�[�h�A�E�g�̃����O�`�󐧌�
    float ringFade = smoothstep(radius, radius - 0.05f, dist); // �����O�ɍ��킹�ăA���t�@�l�𒲐�

    // �����O�̌`�ɉ����ĉ摜�̃A���t�@�l�𐧌䂵�ăt�F�[�h�C���E�t�F�[�h�A�E�g
    output.a *= ringFade;

    // �t�F�[�h���ʂ𔽉f�����摜���o��
    return output;

}