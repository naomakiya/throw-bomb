/*
  @file  PlayerIdling.h
  @brief プレイヤ待機状態クラス
*/
#pragma once
#ifndef PLAYER_IDLING_DEFINED
#define PLAYER_IDLING_DEFINED
#include"Interface/IPlayerState.h"

class CommonResources;
class PlayerState;
class Wall;

class PlayerIdling : public IPlayerState
{
public:
    // コンストラクタ
    PlayerIdling(PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
    // デストラクタ
    ~PlayerIdling();
    // 初期化する
    void Initialize(CommonResources* resources);
    // 事前更新する
    void PreUpdate();
    // 更新する
    void Update(const float& elapsedTime);
    // 事後更新する
    void PostUpdate();
    // 描画する
    void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
    // 後処理を行う
    void Finalize();

private:
    // 共通リソース
    CommonResources* m_commonResources;
    //プレイシーン
    PlayerState* m_player;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Playerの顔モデル
    std::unique_ptr<DirectX::Model> m_playerface;
    // Playerのモデル
    std::unique_ptr<DirectX::Model> m_playerfaceBody;
    // Playerの左手モデル
    std::unique_ptr<DirectX::Model> m_playerHandL;
    // Playerの右手モデル
    std::unique_ptr<DirectX::Model> m_playerHandR;
    // クォータニオン：モデルの回転を制御する
    DirectX::SimpleMath::Quaternion m_rotate;
    // バウンディングスフィア
    DirectX::BoundingSphere m_boundingSphere;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    //手を動かす
    float m_hand;
    //時間
    float m_time;
    //モデルの大きさ
    float m_modelScale;
    // 総時間
    float m_totalSeconds;
    // アニメーション用の変数
    float m_breathingFactor; 
};
#endif		// PLAYER_IDLING_DEFINED