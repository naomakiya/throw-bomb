/*
  @file  EnemyExist.cpp
  @brief 敵の生存状態クラス
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/Resources.h"
#include"Game/Enemy/EnemyState.h"
#include"Game/Enemy/State/EnmyExist.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Wall/Wall.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemyExist::EnemyExist(EnemyState* enemyState, 
    const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_EnemyModel{},
	m_model{},
	m_position{},
    m_wall{wall},
    m_time{0.0f},
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale())
{

}

//---------------------------------------------------------
// デストラクト
//---------------------------------------------------------
EnemyExist::~EnemyExist()
{
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void EnemyExist::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
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
void EnemyExist::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_time = 0.0f;
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void EnemyExist::Update(const float& elapsedTime)
{
    //時間経過
    m_time += elapsedTime;
    //３秒経ったら状態の変更
    if (m_exist)
    {
        m_enemy->ChangeState(m_enemy->GetEnemyMovement());
    }
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void EnemyExist::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void EnemyExist::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{

    auto states = m_commonResources->GetCommonStates();
    // ワールド行列を更新する
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006f);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
    //モデルの描画
    model.Draw(context, *states, world, view, projection);

}

//---------------------------------------------------------
// 処理
//---------------------------------------------------------
void EnemyExist::Finalize()
{
}