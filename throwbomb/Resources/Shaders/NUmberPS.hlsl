
#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

static int digitCount = 3;

float distortion(float2 center, float r, float d_r, float2 uv, float aspect)
{
    float2 uv2 = center;
    uv2.x *= aspect;

    uv.x *= aspect;

    float dist = distance(uv2, uv);

    return 1 - smoothstep(r, d_r, dist);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float2 uv = input.Tex;

    // �e�N�X�`���̕��ƍ����i����\������e�N�X�`���������j
    float w = 5.0f; // �������̕�����
    float h = 2.0f; // �c�����̕�����

    // ���Ԃ𐮐���
    float m_time = floor(time.x);

    // �K�v�Ȍ������v�Z�i��: 123 -> 3���j
    int m_count = max(1, floor(log10(m_time + 1)) + 1);

    // �����J���[�𓧖��ɐݒ�
    float4 color = float4(0, 0, 0, 0);
 
    // �Q����\��
    for (int i = 0; i < digitCount; i++)
    {
        // 1���ځii == 0�j���X�L�b�v
        if (i == 0)
            continue;
        // ���݂̌��̐������v�Z�i1�̈ʁA10�̈ʁA100�̈ʁc�j
        float digit = floor((m_time / pow(10, i)) % 10);

        // �e����UV�𒲐�
        float2 digitUV = uv;

        // �����Ƃ̈ʒu���v�Z�i�E���珇�ɔz�u�j
        digitUV.x = uv.x * w - (4 - i); // �E�[���獶��
        if (digitUV.x < 0.0 || digitUV.x >= 1.0)
            continue; // �\���͈͊O�̓X�L�b�v

        digitUV.x /= w; // ���������e�N�X�`���������ɍ��킹��
        digitUV.y /= h; // �c�������e�N�X�`���������ɍ��킹��

        // ���݂̌��̐����Ɋ�Â��A�e�N�X�`����UV�I�t�Z�b�g���v�Z
        digitUV.x += (1.0 / w) * (digit % w);
        digitUV.y += (1.0 / h) * floor(digit / w);
        
       // float4 digitColor = tex.Sample(samLinear, digitUV);
        float4 digitColor = tex.SampleLevel(samLinear, digitUV, 0);
        color = digitColor;

    }

    return color;
}