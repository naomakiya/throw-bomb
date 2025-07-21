/*
    @file	Floor.h
    @brief	床クラス
*/
#pragma once
#ifndef FLOOR_DEFINED
#define FLOOR_DEFINED
#include <GeometricPrimitive.h>

class  CommonResources;

class Floor
{
public:
    // 横幅
    static constexpr float FLOORWIDTH = 100.0f;
    // 奥幅
    static constexpr float FLOORDEPT = 100.0f;

public:
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

private:
    // 共通リソース
    CommonResources* m_commonResources;
    //	テクスチャハンドル 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
    // 床の形状データ
    std::unique_ptr<DirectX::GeometricPrimitive> m_floorPrimitive;
    //バウディングボックス
    DirectX::BoundingBox m_boundingBox;
    // テクスチャーの座標
    DirectX::SimpleMath::Vector3 m_position;
};

#endif  //FLOOR_DEFINED