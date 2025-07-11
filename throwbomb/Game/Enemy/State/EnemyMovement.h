/*
  @file  EnemyMovement.h
  @brief 敵の移動クラス
*/

#pragma once
#ifndef ENEMY_MOVEMENT_DEFINED
#define ENEMY_MOVEMENT_DEFINED
#include "Interface/IEnemyState.h"
#include "Interface/IPatrolStrategy.h"

// 前方宣言
class CommonResources;
class EnemyState;
class Wall;

class EnemyMovement :public IEnemyState
{
    
public:
    // スケールを取得する
    float GetScale() const { return m_scale; }
    // 位置を取得する
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    // 目的地の位置を設定
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //敵の生存取得
    bool GetExist() const { return m_exist; }
    //敵の生存設定
    void SetExist(const bool exist) { m_exist = exist; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // バウンディングボックスを設定する
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
    // 現在の向いている方向を取得する
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
    // パトロール戦略を設定
    void SetPatrolStrategy(std::unique_ptr<IPatrolStrategy> strategy) {
        m_patrolStrategy = std::move(strategy);
    }

public:
    // コンストラクタ
    EnemyMovement(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath);
    // デストラクタ
    ~EnemyMovement();
    // 初期化する
    void Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos);
    // 事前更新する
    void PreUpdate();
    // 更新する
    void Update(const float&  elapsedTime);
    // 事後更新する
    void PostUpdate();
    // 描画する
    void Render(ID3D11DeviceContext* context,
         const DirectX::SimpleMath::Matrix& view,
         const DirectX::SimpleMath::Matrix& projection,
         const DirectX::Model& model);
    // 後処理を行う
    void Finalize();
    // 向かう場所の設定
    void SetNearestGoal();
    //HPを減らす
    void HPDown();
    // 最も遠いゴール
    int GetFurthestGoalFrom();
    // バウンディング系統の更新
    void UpdateBounding();
    // パトロール
    void UpdateDirectionAndRotation(float elapsedTime);
    // 壁の当たり判定
    void CheckWallCollision();
    // ボムの回避行動
    void CheckAvoidanceFromBombs();
    // プレイヤー関係の当たり判定
    void CheckCollisionWithPlayer();
    // 敵の同期
    void SyncWithEnemyState();

private:
    // 共通リソース
    CommonResources* m_commonResources;
    // 敵
    EnemyState* m_enemy;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // 移動する位置
    std::unique_ptr<IPatrolStrategy> m_patrolStrategy;

private:
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
	// ワールドマトリックス
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// バウンディングスフィア
	DirectX::BoundingSphere m_boundingSphere;
    // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotate; 
    // 現在のゴール番号
    unsigned int m_currentGoalNo;
    // 巡回経路
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath; 
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    //大きさ
    float m_scale;
    //生存
    bool m_exist;
    // ヒットフラグ
    bool m_isHit;
    // 移動速度
    float m_moveSpeed;
    //モデルスケール
    float m_modelScale;
    // ゴール到着しきい値
    float goalArrivalThreshold;

};
#endif		// ENEMYMOVEMENT_DEFINED