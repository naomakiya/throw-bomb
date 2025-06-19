/*
  @file  BomState.cpp
  @brief ボムの状態クラス
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include"Game/Bom/BomState.h"
#include"Game/Bom/State/BomExist.h"
#include"Game/Bom/State/BomMovement.h"
#include"Game/Bom/State/BomExplosion.h"
#include"Game/Bom/State/BomStay.h"
#include"Game/Wall/Wall.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
BomState::BomState(PlayerState* player,const std::vector<std::unique_ptr<Wall>>& wall)
	: m_commonResources{ nullptr }
	, m_wall{ wall }
	, m_player{ player }
	, m_currentState{ nullptr }
	, m_bomPresent{ IBomState::BomtPresent::Exist }
	, m_BoundingSphere{	}
	, m_position{ 0.0f }
	, m_velocity{ 0.0f }
	, m_explosionTimer{ 0.0f }
	, m_scale{ 1.0f }
	, m_currentAngle{ 0.0f }
	, m_isExist{ false }
	, m_isExploded{ false }
	, m_isBounce{ false }
{
	
	
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
BomState::~BomState()
{
	this->Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void BomState::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//爆弾の生存
	m_bomExist = std::make_unique<BomExist>(this,m_player);
	m_bomExist->Initialize(m_commonResources);
	//爆弾の動作
	m_bomMovement = std::make_unique<BomMovement>(this, m_player, m_wall);
	m_bomMovement->Initialize(m_commonResources);
	//爆弾の爆発
	m_bomExplosion = std::make_unique<BomExplosion>(this, m_wall, m_player);
	m_bomExplosion->Initialize(m_commonResources);
	//爆弾の待ち
	m_bomStay = std::make_unique<BomStay>(this, m_player, m_wall);
	m_bomStay->Initialize(m_commonResources);
	//現在の状態
	m_currentState = m_bomExist.get();
}

//---------------------------------------------------------
// 状態の変化
//---------------------------------------------------------
void BomState::ChangeState(IBomState* newBomState)
{
	// 新規の状態遷移前に事後更新を行う
	m_currentState->PostUpdate();
	// 新規の状態を現在の状態に設定する
	m_currentState = newBomState;
	// 新規の状態遷移後に事前更新を行う
	m_currentState->PreUpdate();
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void BomState::Update(const float& elapsedTime)
{
	m_explosionTimer -= elapsedTime;
	//現在の状態の更新
	m_currentState->Update(elapsedTime);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void BomState::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	//現在の状態の描画
	m_currentState->Render(view, projection);
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void BomState::Finalize()
{
	m_bomExplosion.reset();
	m_bomExist.reset();
	m_bomMovement.reset();
	m_bomStay.reset();
}