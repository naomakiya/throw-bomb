// OPFadePS.hlsl
#include "Fade.hlsli"
#define MAX_EXPLOSIONS 5



cbuffer ExplosionBuffer : register(b1)
{
    float4 explosionPositions[MAX_EXPLOSIONS];
};



Texture2D tex : register(t0);
SamplerState samLinear : register(s0);
extern float4x4 g_worldViewProjection;

//time.���ɔ�����Ԃ��L�[�v�����܂��B�@�O���������Ă�A�P���������Ă��Ȃ�
float4 main(PS_INPUT input) : SV_TARGET
{
    //float4 texColor = tex.Sample(samLinear, input.Tex);
    //float alpha = 1.0f;

    //for (int i = 0; i < MAX_EXPLOSIONS; i++)
    //{
    //    // z�ɔ����t���O�i0 = �����ς݁j
    //    if (explosionPositions[i].z >= 1.0f)
    //    {
    //        continue;
    //    }
        
    //    float2 centerUV = explosionPositions[i].xy;
        
    //    float dist = distance(input.Tex.xy, centerUV);
        
    //    // ���a�F�Ōゾ���J�X�^���A����ȊO��0.5
    //    float radius = (i == MAX_EXPLOSIONS - 1) ? 10.0f : 0.5f; 
        
    //    float fade = smoothstep(0.0f, radius, dist);
        
        
    //    alpha *= fade ;
    //    texColor.a *= alpha ;
        
        
    //}

    //return texColor;
    
    
    
    float4 texColor = tex.Sample(samLinear, input.Tex);
    float alpha = 1.0f;

    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        // �����Ȕ����͖���
        if (explosionPositions[i].z >= 1.0f){
            continue;
        }

        float2 centerUV = explosionPositions[i].xy;

        // ���X�ɍL���锼�a�i���Ԃɔ��j �����x������
        float baseRadius = (i == MAX_EXPLOSIONS - 1) ? 15.0f : 0.5f;
        float radius = saturate( time.x/ 3.0f) * baseRadius;
        // �� 2.0f �͍L����X�s�[�h�̌W���i���Ԃ�2�b�ōő唼�a�ɒB����j

        float dist = distance(input.Tex.xy, centerUV);
        float fade = smoothstep(0.0f, radius, dist);

        alpha *= fade;
        
    }

    texColor.a *= alpha;
    return texColor;
}
