/*
  @file  EnemyStop.cpp
  @brief 敵の止まるクラス
*/
#include "pch.h"
#include "Framework/DeviceResources.h"
#include "Framework/CommonResources.h"
#include "Game/Enemy/State/EnmyStop.h"
#include "Game/Enemy/State/EnmyExist.h"
#include "Game/Enemy/EnemyState.h"
#include "Game/UI/Star.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
 EnemyStop:: EnemyStop(EnemyState* enemyState, 
    const std::vector<std::unique_ptr<Wall>>& wall)
	:
	 m_commonResources{ nullptr },
	 m_enemy{ enemyState },
	 m_wall{ wall },
	 m_star{ nullptr },
	 m_boundingBox{},
	 m_position{ m_enemy->GetPosition() },
	 m_stoptime{0},
     m_isExist( m_enemy->GetExist() )
	


{

}

//---------------------------------------------------------
// デストラクト
//---------------------------------------------------------
 EnemyStop::~ EnemyStop()
{
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void  EnemyStop::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	// 混乱時の星の生成
	m_star = std::make_unique<Star>();
	m_star->Create(m_commonResources->GetDeviceResources());
}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void  EnemyStop::PreUpdate()
{	
	// 止める時間の初期化
	m_stoptime = 0.0f;
	// 現在の位置情報の取得
	m_position = m_enemy->GetPosition();
	// 現在のバウディングＢＯＸの取得
	m_boundingBox = m_enemy->GetBoundingBox();
	// 星を配置する
	m_star->SetOn(true);
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void  EnemyStop::Update(const float& elapsedTime)
{
	// 時間経過を刺せる
	m_stoptime += elapsedTime;
	//  星パーティクルの更新
	m_star->Update(elapsedTime,m_position);
   //2秒以上止まったら動けるようにする
	if (m_stoptime > 2.0f){
		// 生存状態に変える
		m_enemy->ChangeState(m_enemy->GetEnemyExist());
	}
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void  EnemyStop::PostUpdate()
{
	// 現在の位置情報を設定する
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void  EnemyStop::Render(ID3D11DeviceContext* context, DirectX::CommonStates* states,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
	// ワールド行列を更新する
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(EnemyState::ENEMYMODELSCALE);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	// モデルを表示する
	model.Draw(context, *states, world, view, projection);
	//  パーティクルの描画
	m_star->Render(view, projection);
}

//---------------------------------------------------------
// 処理
//---------------------------------------------------------
void  EnemyStop::Finalize()
{
}