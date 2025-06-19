#include "fade.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    
     // 時間に基づいて使用するテクスチャを切り替え (time.xが30秒以下かどうかで選択)
    float useTex2 = step(1.0, time.y); // 1ならtex2を選択

    // テクスチャの選択（
    float4 output = lerp(tex.Sample(samLinear, input.Tex), tex2.Sample(samLinear, input.Tex), useTex2);
	//	画像表示
    //float4 output = tex.Sample(samLinear, input.Tex);

    // アスペクト比の調整 (画面が1280x720の場合)
    float aspect = 1280.0f / 720.0f;
    float2 uv = input.Tex;
    uv.x *= aspect; // 横方向にアスペクト比を調整

    // 画面の中央の座標を定義
    float2 center = float2(0.5f * aspect, 0.5f);

    // 現在のピクセル位置と画面中心との距離を計算
    float dist = distance(uv, center);

    // 時間に基づいたリングサイズの動的変化 (sin関数で時間の進行に従って変化)
    float timeFactor = sin(time.x / 1.0f); // 0から1までの値を繰り返す
    float radius = lerp(0.0f, 1.25f, timeFactor); // 0.0から1.25までリングの半径を変化

    // フェードアウトのリング形状制御
    float ringFade = smoothstep(radius, radius - 0.05f, dist); // リングに合わせてアルファ値を調整

    // リングの形に応じて画像のアルファ値を制御してフェードイン・フェードアウト
    output.a *= ringFade;

    // フェード効果を反映した画像を出力
    return output;

}