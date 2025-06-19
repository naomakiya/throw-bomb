/*
  @file  EnemyPointMovement.cpp
  @brief 敵が目的の場所に移動状態クラス
*/
#include "pch.h"
#include "Game/Enemy/State/EnemyPointMovement.h"
#include "Game/Enemy/EnemyState.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Framework/Resources.h"
#include "Game/Wall/Wall.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Game/Enemy/State/EnemyTracking.h"
#include "Game/Enemy/State/EnemySearch.h"
#include "Game/Enemy/State/EnmyStop.h"
#include "Game/Player/PlayerState.h"
#include "Game/Collision/Collision.h"
#include "Game/Bom/BomState.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemyPointMovement::EnemyPointMovement(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall)
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
    m_modelScale(0.006f)
{
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
EnemyPointMovement::~EnemyPointMovement()
{
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void EnemyPointMovement::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	// バウンディングボックス
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);

}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void EnemyPointMovement::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_patrolPathPos = m_enemy->GetPointPosition();
}
 
//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void EnemyPointMovement::Update(const float& elapsedTime)
{
   
    // 敵が存在しない場合は処理を行わない
    if (!m_exist ) return;

    // バウンディングの更新
    UpdateBounding();
    // パトロール処理
    UpdateDirectionAndRotation(elapsedTime);
   //壁の当たり判定
    CheckWallCollision();
    // 敵の同期
    SyncWithEnemyState();
   
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void EnemyPointMovement::PostUpdate()
{
    
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void EnemyPointMovement::Render(ID3D11DeviceContext* context,
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
void EnemyPointMovement::Finalize()
{
}

void EnemyPointMovement::HPDown()
{
    m_enemy->SetExist(false);
}

void EnemyPointMovement::UpdateBounding()
{
    // 位置をバウンディングボックス・スフィアの設定
    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;
}

void EnemyPointMovement::UpdateDirectionAndRotation(float elapsedTime)
{
    // 現在位置と目的地の間を移動するロジック
    

    // プレイヤーの位置に向かって移動する処理
    DirectX::SimpleMath::Vector3 targetPos = m_patrolPathPos;
    // 高さ補正
    targetPos.y += 1.0f;

    DirectX::SimpleMath::Vector3 direction = targetPos - m_position;
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
    float speed = 2.0f;
    m_position += direction * speed * elapsedTime;

    m_enemy->SetPosition(m_position);

    // 到達チェック（例）
    if ((m_patrolPathPos - m_position).Length() < 1.5f) {
        // 目的地に到着したらストップステートに遷移するなど
        m_enemy->ChangeState(m_enemy->GetEnemySearch());
    }

}

void EnemyPointMovement::CheckWallCollision()
{
    // 壁との当たり判定
    for (const auto& wall : m_wall)
    {
        mylib::Collision::CheckHit(m_boundingBox, wall->GetBoundingBox());
    }
}

void EnemyPointMovement::SyncWithEnemyState()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
    m_enemy->SetBoundingSphere(m_boundingSphere);
}
