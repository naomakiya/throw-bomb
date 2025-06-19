/*
  @file Player.cpp
  @brief プレイヤクラス
*/
#include "pch.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include"Game/Enemy/EnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/Resources.h"
#include"Game/Wall/Wall.h"
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <DirectXCollision.h>
#include <queue>
#include <random>
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/recastnavigation/Detour/Include/DetourNavMesh.h"
#include "Libraries/recastnavigation/Detour/Include/DetourNavMeshQuery.h"
#include"Libraries/MyLib/CollisionMesh.h"
#include "Libraries/MyLib/InputManager.h"
#include"Game/Enemy/State/EnemyTracking.h"

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
    gen{},
    m_pauseChance(0.0f, 1.0f),
    m_isHit{false},
    m_collisionMesh{},
   /* m_navMesh{},
    m_navQuery{},*/
    m_patrolPath(patrolPath),
    m_currentGoalNo(0)
{
	//// グラフィックスを取得する
	//m_graphics = Graphics::GetInstance();
	//// コンテキストを取得する
	//m_context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
}

EnemyMovement::~EnemyMovement()
{
}

void EnemyMovement::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    //auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// モデルを読み込む準備
	//std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	//fx->SetDirectory(L"Resources/Models/Enemy");

	////// モデルをロードする
 //   m_enemyModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Enemy/Enemy.cmo", *fx);

	// バウンディングボックス
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);

    m_boundingSphere.Center = pos;
    m_boundingSphere.Radius = 2.5f;

    // メッシュを生成する
   /* m_collisionMesh = std::make_unique<mylib::CollisionMesh>();
    m_collisionMesh->Initialize(device, context, L"Floor");*/

    //m_enemyModel->UpdateEffects([&](DirectX::IEffect* effect)
    //    {
    //        DirectX::DX11::BasicEffect* basic = dynamic_cast<DirectX::DX11::BasicEffect*>(effect);
    //        if (basic != nullptr)
    //        {
    //            //basic->SetEmissiveColor(DirectX::Colors::White);
    //            basic->SetDiffuseColor(DirectX::Colors::Black);

    //        }
    //    });

    // 最初の目標パスを設定する
    SetNearestGoal();
}


void EnemyMovement::PreUpdate()
{
    m_position = m_enemy->GetPosition();
}

void EnemyMovement::Update(const float& elapsedTime,
    DirectX::SimpleMath::Vector3 target)
{
   
    // 敵が存在しない場合は処理を行わない
    if (!m_exist) return;

    if (m_patrolPath.empty())
    {
        return;
    }

    // 位置をバウンディングボックスに設定
    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;

    // 移動速度
    float moveSpeed = 1.5f;

    // 現在のゴールに向かって移動する
    DirectX::SimpleMath::Vector3 targetPosition = m_patrolPath[m_currentGoalNo];
    DirectX::SimpleMath::Vector3 direction = targetPosition - m_position;

    // 方向を正規化する
    direction.Normalize();

    // 進行方向に基づく回転を計算
    if (direction.LengthSquared() > 0.0f)
    {
        m_rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
            atan2(direction.x, direction.z),  // Yaw (Y軸周りの回転)
            0.0f,                             // Pitch
            0.0f                              // Roll
        );
    }

    // 現在の位置を更新する
    m_position += direction * moveSpeed * elapsedTime;

    // 目標位置までの距離を計算する
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(m_position, targetPosition);

    // ゴールに到達したら、次のゴールに移動する
    if (distanceToTarget < 0.5f)
    {
        m_currentGoalNo++;
        if (m_currentGoalNo >= m_patrolPath.size())
        {
            m_currentGoalNo = 0;
        }
    }

    // 壁との当たり判定
    for (const auto& wall : m_wall)
    {
        CheckHit(wall->GetBoundingBox());
    }

    // プレイヤーとの判定
    if (PlayerCheckHit(m_enemy->GetPlayer()->GetBoundingBox()))
    {
        m_enemy->ChangeState(m_enemy->GetEnemyTracking());
    }

    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
    m_enemy->SetBoundingSphere(m_boundingSphere);
}


void EnemyMovement::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

//描画
void EnemyMovement::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();

    // ワールド行列を更新する
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006f);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

    // モデルを表示する
    model.Draw(context, *states, world, view, projection);
 
}

void EnemyMovement::Finalize()
{
}

//---------------------------------------------------------
// 衝突判定する
//---------------------------------------------------------
void EnemyMovement::CheckHit(DirectX::BoundingBox boundingBox)
{
    // バウンディングボックス同士の衝突判定をとる
    m_isHit = m_boundingBox.Intersects(boundingBox);

    // ヒットしていなければ終わり
    if (!m_isHit) { return; }

    // 衝突時、プレイヤーキャラクターを押し戻す処理

    // AABB用のmin/maxを計算する
    DirectX::SimpleMath::Vector3 aMin = DirectX::SimpleMath::Vector3(m_boundingBox.Center) - DirectX::SimpleMath::Vector3(m_boundingBox.Extents);
    DirectX::SimpleMath::Vector3 aMax = DirectX::SimpleMath::Vector3(m_boundingBox.Center) + DirectX::SimpleMath::Vector3(m_boundingBox.Extents);
    DirectX::SimpleMath::Vector3 bMin = DirectX::SimpleMath::Vector3(  boundingBox.Center) - DirectX::SimpleMath::Vector3(  boundingBox.Extents);
    DirectX::SimpleMath::Vector3 bMax = DirectX::SimpleMath::Vector3(  boundingBox.Center) + DirectX::SimpleMath::Vector3(  boundingBox.Extents);

    // 各軸の差分を計算する
    float dx1 = bMax.x - aMin.x;
    float dx2 = bMin.x - aMax.x;
    float dy1 = bMax.y - aMin.y;
    float dy2 = bMin.y - aMax.y;
    float dz1 = bMax.z - aMin.z;
    float dz2 = bMin.z - aMax.z;

    // 各軸について、絶対値の小さい方を軸のめり込み量とする：AABBの重なった部分を特定する
    float dx = std::abs(dx1) < std::abs(dx2) ? dx1 : dx2;
    float dy = std::abs(dy1) < std::abs(dy2) ? dy1 : dy2;
    float dz = std::abs(dz1) < std::abs(dz2) ? dz1 : dz2;

    // 押し戻しベクトル
    DirectX::SimpleMath::Vector3 pushBackVec = DirectX::SimpleMath::Vector3::Zero;

    // めり込みが一番小さい軸を押し戻す
    if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz))
    {
        pushBackVec.x += dx;
    }
    else if (std::abs(dz) <= std::abs(dx) && std::abs(dz) <= std::abs(dy))
    {
        pushBackVec.z += dz;
    }
    else
    {
        pushBackVec.y += dy;
    }

    // 押し戻す
    m_position += pushBackVec;
    m_boundingBox.Center = m_boundingBox.Center + pushBackVec;
}

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

bool EnemyMovement::PlayerCheckHit(DirectX::BoundingBox boundingBox)
{
    // AABBの最も近い点を計算する
    DirectX::SimpleMath::Vector3 closestPoint = m_boundingSphere.Center;

    // 各軸について、スフィアの中心がAABBの外側にある場合、その軸のAABBの端点を使用する
    if (m_boundingSphere.Center.x < boundingBox.Center.x - boundingBox.Extents.x)
        closestPoint.x = boundingBox.Center.x - boundingBox.Extents.x;
    else if (m_boundingSphere.Center.x > boundingBox.Center.x + boundingBox.Extents.x)
        closestPoint.x = boundingBox.Center.x + boundingBox.Extents.x;

    if (m_boundingSphere.Center.y < boundingBox.Center.y - boundingBox.Extents.y)
        closestPoint.y = boundingBox.Center.y - boundingBox.Extents.y;
    else if (m_boundingSphere.Center.y > boundingBox.Center.y + boundingBox.Extents.y)
        closestPoint.y = boundingBox.Center.y + boundingBox.Extents.y;

    if (m_boundingSphere.Center.z < boundingBox.Center.z - boundingBox.Extents.z)
        closestPoint.z = boundingBox.Center.z - boundingBox.Extents.z;
    else if (m_boundingSphere.Center.z > boundingBox.Center.z + boundingBox.Extents.z)
        closestPoint.z = boundingBox.Center.z + boundingBox.Extents.z;

    // 最も近い点とスフィアの中心との距離を計算する
    DirectX::SimpleMath::Vector3 distanceVec = closestPoint - m_boundingSphere.Center;
    float distanceSq = distanceVec.LengthSquared();

    // 距離の二乗がスフィアの半径の二乗以下であれば、衝突が発生している
    return distanceSq <= m_boundingSphere.Radius * m_boundingSphere.Radius;
}
