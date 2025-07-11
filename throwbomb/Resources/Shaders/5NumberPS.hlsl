
#include "Particle.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

static int digitCount = 7;

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

    float digitWidth = 1.0 / digitCount;

    float4 color = float4(0, 0, 0, 0);

    for (int i = 0; i < digitCount; i++)
    {
        // �e���̉��͈́i�����E�j
        float xMin = digitWidth * i;
        float xMax = xMin + digitWidth;

        // ���̃s�N�Z�����\������͈͊O�Ȃ�X�L�b�v
        if (uv.x < xMin || uv.x >= xMax)
            continue;

        // �E���獶�ɐ�������ׂ�ii=0�����[�AdigitIndex=6��i=6��1�̈ʁj
        int digitIndex = digitCount - 1 - i;

        float digit = floor((m_time / pow(10, digitIndex)) % 10);

        // �����ł�UV�����[�J���ɃX�P�[�����O
        float2 digitUV;
        digitUV.x = (uv.x - xMin) / digitWidth;
        digitUV.y = uv.y;

        // �e�N�X�`����UV�ɕϊ�
        digitUV.x /= w;
        digitUV.y /= h;
        digitUV.x += (1.0 / w) * (digit % w);
        digitUV.y += (1.0 / h) * floor(digit / w);

        float4 digitColor = tex.SampleLevel(samLinear, digitUV, 0);
        color = digitColor;
    }


    return color;
}