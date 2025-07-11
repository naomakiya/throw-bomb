/*
  @file  BomMovement.h
  @brief ボム移動クラス
*/
#pragma once
#ifndef BOM_MOVEMENT_DEFINED
#define BOM_MOVEMENT_DEFINED

#include"Interface/IBomState.h"

class CommonResources;
class PlayerState;
class BomState;
class CollisionDebugRenderer;

class BomMovement : public IBomState
{
public:
     // コンストラクタ
     BomMovement(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
     // デストラクタ
     ~BomMovement();
     // 初期化する
     void Initialize(CommonResources* resources) override;
     // 事前更新する
     void PreUpdate();
     void Projection(const DirectX::SimpleMath::Vector3& playerForwardDirection);
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
     //爆弾のバウンド
     void HandleCollision();

public:
    // 反発係数
    static constexpr float BOUNCEFACTOR = 0.8f;
    // カウントダウン
    static constexpr float COUNTDOWNTIME = 3.5f;
    // 初期速度
    static constexpr float LAUNCHSPEED = 10.0f;
    // 地面の高さ
    static constexpr float GROUNDHEIGHT = 0.7f;
    // 最少の高さ
    static constexpr float MINHEIGHT = 0.0f;

private:
    // 共通リソース
    CommonResources* m_commonResources;
    // プレイヤー
    PlayerState* m_player;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // ボム
    BomState* m_bomState;
    // コリジョンデバックレンダー
    std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;

    //ボムのモデル
    std::unique_ptr<DirectX::Model> m_bomModel;
    // バウンディングスフィア
    DirectX::BoundingSphere m_boundingSphere;

    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    //速度
    DirectX::SimpleMath::Vector3 m_velocity;
    //加速度
    DirectX::SimpleMath::Vector3 m_gravity;
    

    //バウンドしているか
    bool m_isBounce;
};
#endif		// BOM_MOVEMENT_DEFINED