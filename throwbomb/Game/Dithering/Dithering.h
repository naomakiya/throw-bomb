/*
    @file	Dithering.h
    @brief	ディザリングクラス
*/
#pragma once
#ifndef DITHERING_DEFINED
#define DITHERING_DEFINED
// 前方宣言
namespace DX
{
    class DeviceResources;
}

class Dithering
{
private:

    // オブジェクト情報を保持する構造体
    struct ObjectInfo
    {
        DirectX::SimpleMath::Matrix world;      // ワールド変換行列
        DirectX::SimpleMath::Vector3 position;  // オブジェクトのワールド座標
        DirectX::Model* model = nullptr;                  // 描画する3Dモデル
    };

    // ライトの定数バッファ用構造体
    struct cbLight
    {
        DirectX::XMVECTOR lightDirection;		// ライトの方向
        DirectX::XMMATRIX viewMatrix;			// ビュー行列
        DirectX::XMVECTOR cameraPosition;		// カメラ座標
    };
public:
// コンストラクタ
    Dithering(ID3D11Device1* device);
    // デストラクタ
    ~Dithering();
    // オブジェクトを描画するメソッド
    void RenderObjects(ID3D11DeviceContext* context, DirectX::DX11::CommonStates* states, DirectX::SimpleMath::Matrix projection, DirectX::SimpleMath::Matrix view,
        DirectX::SimpleMath::Vector3 eye,
        DirectX::SimpleMath::Matrix worlds,
        DirectX::SimpleMath::Vector3 position,
        DirectX::Model* model);
private:
    // シェーダをロードする
    void LoadShader(ID3D11Device* device);
    // オブジェクトのリスト
    std::vector<ObjectInfo> m_objects;
    //頂点シェイダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    //ピクセルシェイダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    // 定数バッファ用のバッファオブジェクト
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

    
};
#endif //DITHERING_DEFINED


