/*
  @file Player.h
  @brief プレイヤクラス
*/
#pragma once
#ifndef ENEMYMOVEMENT_IDLING_DEFINED
#define ENEMYMOVEMENT_IDLING_DEFINED
#include "Interface/IEnemyState.h"
#include <random>
#include "Libraries/recastnavigation/Detour/Include/DetourNavMesh.h"
#include "Libraries/recastnavigation/Detour/Include/DetourNavMeshQuery.h"
#include"Libraries/MyLib/CollisionMesh.h"

// 前方宣言
class CommonResources;
class EnemyState;
class Wall;
class mylib::CollisionMesh;

// A*アルゴリズムでのパス（経路）型
using Path = std::vector<DirectX::SimpleMath::Vector3>;

class EnemyMovement :public IEnemyState
{
public:
    // スケールを取得する
    float GetScale() const { return m_scale; }
    // スケールを設定する
    void SetScale(const float scale) { m_scale = scale; }
    // 位置を取得する
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //敵の生存取得
    bool GetExist() const { return m_exist; }
    //敵の生存設定
    void SetExist(const bool exist) { m_exist = exist; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // バウンディングボックスを取得する
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
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
     void Update(const float&  elapsedTime,
         DirectX::SimpleMath::Vector3 target);
     // A*アルゴリズムでの移動
     Path MoveTo(const DirectX::SimpleMath::Vector3& targetPosition);
     // 事後更新する
     void PostUpdate();
     // 描画する
     void Render(ID3D11DeviceContext* context,
         const DirectX::SimpleMath::Matrix& view,
         const DirectX::SimpleMath::Matrix& projection,
         const DirectX::Model& model);
     // 後処理を行う
     void Finalize();
     void CheckHit(DirectX::BoundingBox boundingBox);
     void HPDown() {};
     void SetNearestGoal();
     bool PlayerCheckHit(DirectX::BoundingBox boundingBox);
private:
    // 共通リソース
    CommonResources* m_commonResources;
    //
    EnemyState* m_enemy;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemyのモデル
    //std::unique_ptr<DirectX::Model> m_enemyModel;
    // コリジョンメッシュ
    std::unique_ptr<mylib::CollisionMesh> m_collisionMesh;
private:
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
	// ワールドマトリックス
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// バウンディングスフィア
	DirectX::BoundingSphere m_boundingSphere;
    // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotation; 
    // 現在のゴール番号
    unsigned int m_currentGoalNo;
    // 巡回経路
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath; 
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    //大きさ
    bool m_scale;
    //生存
    bool m_exist;
    // ヒットフラグ
    bool m_isHit;

    bool m_isPaused;
    float m_pauseTimer;
    float m_pauseDuration;
    float m_timeUntilNextPause;
    // シード値を取得する
    // std::mt19937 で乱数生成器を初期化する
    std::mt19937 gen{ std::random_device{}() };
    std::uniform_real_distribution<float> m_pauseChance;
};
#endif		// PLAYER_DEFINED