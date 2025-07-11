/*
  @file  BomStay.h
  @brief ボムの真下に置くクラス
*/
#pragma once
#ifndef BOM_STAY_DEFINED
#define BOM_STAY_DEFINED

#include"Interface/IBomState.h"

class CommonResources;
class PlayerState;
class BomState;

class BomStay : public IBomState
{
public:
     // コンストラクタ
     BomStay(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
     // デストラクタ
     ~BomStay();
     // 初期化する
     void Initialize(CommonResources* resources) override;
     // 事前更新する
     void PreUpdate();
     // 更新する
     void Update(const float& elapsedTime) override;
     // 事後更新する
     void PostUpdate();
     // 描画する
     void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) override;
     // 後処理を行う
     void Finalize() override;

private:
     //衝突判定
     void HitCheck(DirectX::BoundingBox boundingBox,const bool IsWall);

private:
    // 共通リソース
    CommonResources* m_commonResources;
    // プレイヤー
    PlayerState* m_player; 
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // ボムステート（親）
    BomState* m_bomState;

    //爆弾のモデル
    std::unique_ptr<DirectX::Model> m_bomModel;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    //速度
    DirectX::SimpleMath::Vector3 m_velocity;
    //加速度
    DirectX::SimpleMath::Vector3 m_gravity;

    // バウンディングスフィア
    DirectX::BoundingSphere m_boundingSphere;
};
#endif		//BOM_STAY_DEFINED