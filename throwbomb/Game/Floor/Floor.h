/*
    @file	Floor.h
    @brief	床クラス
*/
#pragma once
#ifndef FLOOR_DEFINED
#define FLOOR_DEFINED

#include <GeometricPrimitive.h>
#include <wrl/client.h>  

class  CommonResources;

class Floor
{
public:
    // テクスチャーの座標と大きさを取得する関数
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
public:
    // コンストラクタ
    Floor();
    //デストラクタ
    ~Floor();
    //初期化
    void Initialize(CommonResources* resources);
    //描画
    void Render(const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& proj);
    //後処理
    void Finalize();

public:
    // 横幅
    static constexpr float FLOORWIDTH = 100.0f;
    // 奥幅
    static constexpr float FLOORDEPT = 100.0f;

private:
    // 共通リソース
    CommonResources* m_commonResources;
    //	入力レイアウト 
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
    //	テクスチャハンドル 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
    //	エフェクト 
    std::unique_ptr<DirectX::AlphaTestEffect> m_BatchEffect;
    // 床の形状データ
    std::unique_ptr<DirectX::GeometricPrimitive> m_floorPrimitive;
    // 深度ステンシルステート
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    //バウディングボックス
    DirectX::BoundingBox m_boundingBox;
    // テクスチャーの座標
    DirectX::SimpleMath::Vector3 m_position;
};

#endif  //FLOOR_DEFINED