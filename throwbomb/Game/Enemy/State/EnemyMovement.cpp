/*
  @file  EnemyMovement.cpp
  @brief 敵の移動状態クラス
*/
#include "pch.h"
#include "Game/Enemy/State/EnemyMovement.h"
#include "Game/Enemy/EnemyState.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Wall/Wall.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Game/Enemy/State/EnemyTracking.h"
#include "Game/Player/PlayerState.h"
#include "Game/Collision/Collision.h"
#include "Game/Bom/BomState.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemyMovement::EnemyMovement(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_position{},
    m_wall{wall},
    m_scale(m_enemy->GetScale()),
    m_exist(m_enemy->GetExist()),
    m_isPaused(),
    m_pauseTimer{},
    m_pauseDuration{},
    m_timeUntilNextPause{},
    m_moveSpeed(2.5f),
    m_isHit{false},
    m_patrolPath(patrolPath),
    m_currentGoalNo(0),
    m_modelScale(0.006f),
    goalArrivalThreshold{0.5f}
{
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
EnemyMovement::~EnemyMovement()
{
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void EnemyMovement::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	// バウンディングボックス
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);

    m_boundingSphere.Center = pos;
    m_boundingSphere.Radius = 2.5f;

    // 最初の目標パスを設定する
    SetNearestGoal();
}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void EnemyMovement::PreUpdate()
{
    m_position = m_enemy->GetPosition();
}
 
//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void EnemyMovement::Update(const float& elapsedTime)
{
   
    // 敵が存在しない場合は処理を行わない または　パスが存在しない場合処理しない
    if (!m_exist || m_patrolPath.empty()) return;

    // バウンディングの更新
    UpdateBounding();
    // パトロール処理
    UpdateDirectionAndRotation(elapsedTime);
   //壁の当たり判定
    CheckWallCollision();
    // ボムの回避行動
    CheckAvoidanceFromBombs();
    //プレイヤーの判定
    CheckCollisionWithPlayer();
    // 敵の同期
    SyncWithEnemyState();
   
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void EnemyMovement::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void EnemyMovement::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();

    // ワールド行列を更新する
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_modelScale);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotate);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

    // モデルを表示する
    model.Draw(context, *states, world, view, projection);
 
}

//---------------------------------------------------------
// 後処理
//---------------------------------------------------------
void EnemyMovement::Finalize()
{
}

//---------------------------------------------------------
// 向かう場所の設定
//---------------------------------------------------------
void EnemyMovement::SetNearestGoal()
{
    if (m_patrolPath.empty())
    {
        m_currentGoalNo = 0;
        return;
    }

    // 現在位置と各パス位置の距離を計算し、最も近い目標を見つける
    float minDistanceSq = FLT_MAX;
    int nearestGoal = 0;

    for (size_t i = 0; i < m_patrolPath.size(); ++i)
    {
        float distanceSq = DirectX::SimpleMath::Vector3::DistanceSquared(m_position, m_patrolPath[i]);
        if (distanceSq < minDistanceSq)
        {
            minDistanceSq = distanceSq;
            nearestGoal = static_cast<int>(i);
        }
    }

    m_currentGoalNo = nearestGoal;
}

void EnemyMovement::HPDown()
{
    m_enemy->SetExist(false);
}
//---------------------------------------------------------
// 最も遠いゴール
//---------------------------------------------------------
int EnemyMovement::GetFurthestGoalFrom()
{
    const auto& playerPos = m_enemy->GetPlayer()->GetPosition();
    float maxDistanceSq = 0.0f;
    int furthestIndex = m_currentGoalNo;

    for (size_t i = 0; i < m_patrolPath.size(); ++i)
    {
        float distSq = DirectX::SimpleMath::Vector3::DistanceSquared(playerPos, m_patrolPath[i]);
        if (distSq > maxDistanceSq)
        {
            maxDistanceSq = distSq;
            furthestIndex = static_cast<int>(i);
        }
    }

    return furthestIndex;
}

void EnemyMovement::UpdateBounding()
{
    // 位置をバウンディングボックス・スフィアの設定
    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;
}

void EnemyMovement::UpdateDirectionAndRotation(float elapsedTime)
{
    // 現在のゴールに向かって移動する
    DirectX::SimpleMath::Vector3 targetPosition = m_patrolPath[m_currentGoalNo];
    DirectX::SimpleMath::Vector3 direction = targetPosition - m_position;

    // 方向を正規化する
    direction.Normalize();

    // 進行方向に基づく回転を計算
    if (direction.LengthSquared() > 0.0f)
    {
        m_rotate = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
            atan2(direction.x, direction.z),  // Yaw (Y軸周りの回転)
            0.0f,                             // Pitch
            0.0f                              // Roll
        );
    }

    // 現在の位置を更新する
    m_position += direction * m_moveSpeed * elapsedTime;

    // 目標位置までの距離を計算する
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(m_position, targetPosition);

    // ゴールに到達したら、次のゴールに移動する
    if (distanceToTarget < goalArrivalThreshold)
    {
        m_currentGoalNo++;
        if (m_currentGoalNo >= m_patrolPath.size())
        {
            m_currentGoalNo = 0;
        }
    }
}

void EnemyMovement::CheckWallCollision()
{
    // 壁との当たり判定
    for (const auto& wall : m_wall)
    {
        mylib::Collision::CheckHit(m_boundingBox, wall->GetBoundingBox());
    }
}

void EnemyMovement::CheckAvoidanceFromBombs()
{
    // 回避条件：ボムが近い
    float avoidRadius = 3.5f;
    for (int i = 0; i < 3; i++)
    {
        float playerDistance = DirectX::SimpleMath::Vector3::Distance(m_position, m_enemy->GetPlayer()->GetBomState(i)->GetPosition());

        if (playerDistance < avoidRadius)
        {
            // プレイヤーから最も遠いパトロール地点に逃げる
            m_currentGoalNo = GetFurthestGoalFrom();
        }
    }
}

void EnemyMovement::CheckCollisionWithPlayer()
{
    // プレイヤーとの判定
    if (mylib::Collision::BoundingCheckCollision(m_enemy->GetPlayer()->GetBoundingBox(), m_boundingSphere))
    {
        m_enemy->ChangeState(m_enemy->GetEnemyTracking());
    }
}

void EnemyMovement::SyncWithEnemyState()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
    m_enemy->SetBoundingSphere(m_boundingSphere);
}
