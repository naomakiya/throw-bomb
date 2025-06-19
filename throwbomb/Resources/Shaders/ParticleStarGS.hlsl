#include "Particle.hlsli"

static const int vnum = 4;

// オフセットUVと頂点のローカル2Dオフセット
static const float2 offset_array[vnum] =
{
    float2(-0.5f, 0.5f), // 左上
	float2(0.5f, 0.5f), // 右上
	float2(-0.5f, -0.5f), // 左下
	float2(0.5f, -0.5f), // 右下
};

[maxvertexcount(vnum)]
void main(
	point PS_INPUT input[1],
	inout TriangleStream<PS_INPUT> output
)
{
	// カメラの右方向ベクトル（列ベクトル参照）
    float3 camRight = float3(matView._11, matView._21, matView._31);
	// カメラの上方向ベクトル
    float3 camUp = float3(matView._12, matView._22, matView._32);

	// スケーリング（input[0].Tex.xに格納されている想定）
    float scale = input[0].Tex.x;

    for (int i = 0; i < vnum; ++i)
    {
        PS_INPUT element = input[0];

		// クアッドのローカル座標（オフセット * スケーリング）
        float2 offset = offset_array[i] * scale;

		// カメラ空間に沿った位置調整（ビルボード）
        float3 worldOffset = offset.x * camRight + offset.y * camUp;

		// 最終ワールド位置
        float3 worldPos = input[0].Pos.xyz + worldOffset;

		// MVP変換
        element.Pos = mul(float4(worldPos, 1.0f), matView);
        element.Pos = mul(element.Pos, matProj);

		// UV座標（[0,1] 範囲に変換）
        element.Tex = offset_array[i] + 0.5f;

        output.Append(element);
    }
    output.RestartStrip();
}