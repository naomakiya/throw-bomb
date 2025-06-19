#include "Particle.hlsli"

static const int vnum = 4;

// �I�t�Z�b�gUV�ƒ��_�̃��[�J��2D�I�t�Z�b�g
static const float2 offset_array[vnum] =
{
    float2(-0.5f, 0.5f), // ����
	float2(0.5f, 0.5f), // �E��
	float2(-0.5f, -0.5f), // ����
	float2(0.5f, -0.5f), // �E��
};

[maxvertexcount(vnum)]
void main(
	point PS_INPUT input[1],
	inout TriangleStream<PS_INPUT> output
)
{
	// �J�����̉E�����x�N�g���i��x�N�g���Q�Ɓj
    float3 camRight = float3(matView._11, matView._21, matView._31);
	// �J�����̏�����x�N�g��
    float3 camUp = float3(matView._12, matView._22, matView._32);

	// �X�P�[�����O�iinput[0].Tex.x�Ɋi�[����Ă���z��j
    float scale = input[0].Tex.x;

    for (int i = 0; i < vnum; ++i)
    {
        PS_INPUT element = input[0];

		// �N�A�b�h�̃��[�J�����W�i�I�t�Z�b�g * �X�P�[�����O�j
        float2 offset = offset_array[i] * scale;

		// �J������Ԃɉ������ʒu�����i�r���{�[�h�j
        float3 worldOffset = offset.x * camRight + offset.y * camUp;

		// �ŏI���[���h�ʒu
        float3 worldPos = input[0].Pos.xyz + worldOffset;

		// MVP�ϊ�
        element.Pos = mul(float4(worldPos, 1.0f), matView);
        element.Pos = mul(element.Pos, matProj);

		// UV���W�i[0,1] �͈͂ɕϊ��j
        element.Tex = offset_array[i] + 0.5f;

        output.Append(element);
    }
    output.RestartStrip();
}