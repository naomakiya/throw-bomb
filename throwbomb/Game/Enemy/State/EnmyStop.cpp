/*
  @file  EnemyStop.cpp
  @brief 敵の止まるクラス
*/
#include "pch.h"
#include "Framework/Resources.h"
#include "Game/Enemy/State/EnmyStop.h"
#include "Game/Enemy/State/EnmyExist.h"
#include "Game/Enemy/EnemyState.h"
#include "Framework/CommonResources.h"
#include "Game/UI/Star.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
 EnemyStop:: EnemyStop(EnemyState* enemyState, 
    const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_EnemyModel{},
	m_model{},
	m_position{},
    m_wall{wall},
	 m_stoptime{0},
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale()),
	 m_star{}
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
void  EnemyStop::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	// バウンディングボックス
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);


	// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/Enemy");

	//// モデルをロードする
	m_enemyModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Enemy/Enemy.cmo", *fx);


	m_star = std::make_unique<Star>();
	m_star->Create(m_commonResources->GetDeviceResources());
}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void  EnemyStop::PreUpdate()
{
	m_stoptime = 0.0f;
	m_position = m_enemy->GetPosition();
	m_star->SetOn(true);
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void  EnemyStop::Update(const float& elapsedTime)
{
	m_stoptime += elapsedTime;

	//  パーティクルの更新
	m_star->Update(elapsedTime,m_position);
   //３秒以上止まったら動けるようにする
	if (m_stoptime > 2.5f)
	{
		m_enemy->ChangeState(m_enemy->GetEnemyExist());
	}
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void  EnemyStop::PostUpdate()
{
    m_enemy->SetPosition(m_position);

	
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void  EnemyStop::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
	auto states = m_commonResources->GetCommonStates();

	// ワールド行列を更新する
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006);
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