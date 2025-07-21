/*
  @file  EnemyExist.cpp
  @brief 敵の生存状態クラス
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Enemy/EnemyState.h"
#include"Game/Enemy/State/EnmyExist.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Wall/Wall.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemyExist::EnemyExist(EnemyState* enemyState)
	:
    m_commonResources{ nullptr },
	m_enemy{ enemyState },
	m_position{ m_enemy->GetPosition() },
    m_time{ 0.0f },
    m_isExist(m_enemy->GetExist())
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
void EnemyExist::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void EnemyExist::PreUpdate()
{
    // 位置情報の更新
    m_position = m_enemy->GetPosition();
    // バウディングBOXの更新
    m_boundingBox = m_enemy->GetBoundingBox();
    //時間の初期化
    m_time = 0.0f;
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void EnemyExist::Update(const float& elapsedTime)
{
    m_time += elapsedTime;
    // いるなら状態変更(少しの間を開けてから移動)
    if (m_isExist && m_time > 0.25f){
        // 動く状態へ
        m_enemy->ChangeState(m_enemy->GetEnemyMovement());
    }
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void EnemyExist::PostUpdate()
{
    // 位置の設定
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void EnemyExist::Render(ID3D11DeviceContext* context, DirectX::CommonStates* states,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    // ワールド行列を更新する
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(EnemyState::ENEMYMODELSCALE);
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