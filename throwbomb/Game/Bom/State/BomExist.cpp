/*
  @file BomExist.cpp
  @brief ボム生存状態クラス
*/
#include "pch.h"
#include"Game/Bom/State/BomExist.h"
#include"Game/Bom/BomState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/DebugString.h"
#include"Game/Wall/Wall.h"
#include"Libraries/MyLib/InputManager.h"
#include"Game/Player/PlayerState.h"
#include"Game/Bom/State/BomMovement.h"
#include"Game/Bom/State/BomStay.h"
#include <Game/ResourceManager/ResourceManager.h>

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
BomExist::BomExist(BomState* bomState,PlayerState* player)
	: m_commonResources(nullptr)
	, m_bom(bomState)
	, m_player(player)
	, m_currentAngle(0.0f)
	, m_isExist(false)
	, m_isSpaceHeld(false)
	, m_isHoldingBom(false)
	, m_holdStartTime(0.0f)
	, m_isAngleIncreasing(false)
	, m_position()
{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
BomExist::~BomExist()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void BomExist::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	// ボムの3Dモデルを読み込む
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Bom").c_str(), *fx);

}

//---------------------------------------------------------
// 事前更新する
//---------------------------------------------------------
void BomExist::PreUpdate()
{
	m_isExist = false;
	m_currentAngle = 0.0f;
	m_bom->SetBomPresent(BomState::Exist);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void BomExist::Update(const float& elapsedTime)
{
	// キーボードステートトラッカーを取得する
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// ボムがまだ存在していない → 状態保持確認
	if (!m_isExist)
	{
		m_isExist = m_bom->GetExist();
		return;
	}

	// ボムを保持中：プレイヤーの位置に追従
	m_position = m_player->GetPosition();
	m_position.y += BomSpawnHeight;
	m_bom->SetPosition(m_position);
	m_isHoldingBom = true;

	// 投げ準備：スペースキーを押した瞬間
	if (kbTracker->pressed.Space && m_isHoldingBom)
	{
		m_isSpaceHeld = true;
		m_holdStartTime = 0.0f;
	}

	// 投げ実行：スペースキー離した瞬間
	if (kbTracker->released.Space && m_isHoldingBom)
	{
		m_bom->ChangeState(m_bom->GetBomMovement());
		return;
	}

	// 置く処理：Xキーでその場に設置
	if (kbTracker->pressed.X && m_isHoldingBom)
	{
		m_bom->ChangeState(m_bom->GetBomStay());
		return;
	}

	// スペースキー長押し処理：角度スイング＋強制投げ判定
	if (m_isSpaceHeld)
	{
		m_holdStartTime += elapsedTime;

		// 角度スイング処理
		if (m_isAngleIncreasing)
		{
			m_currentAngle -= elapsedTime * m_angleSpeed;
			if (m_currentAngle <= 0.0f) m_isAngleIncreasing = false;
		}
		else
		{
			m_currentAngle += elapsedTime * m_angleSpeed;
			if (m_currentAngle >= 90.0f) m_isAngleIncreasing = true;
		}

		// 長押しが10秒超えたら強制投げ
		if (m_holdStartTime >= 10.0f)
		{
			m_isSpaceHeld = false;
			m_bom->ChangeState(m_bom->GetBomMovement());
			return;
		}
	}

	// 投げ角度を共有
	m_bom->SetAngle(m_currentAngle);
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void BomExist::PostUpdate()
{
	m_isSpaceHeld = false;
	m_bom->SetBomPresent(BomState::MOVEMENT);
	m_bom->SetAngle(m_currentAngle);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void BomExist::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	if (!m_isExist) return;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// ワールド行列作成
	DirectX::SimpleMath::Matrix world =
		DirectX::SimpleMath::Matrix::CreateScale(0.005f) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	// ボムの描画
	m_bomModel->Draw(context, *states, world, view, projection);
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void BomExist::Finalize()
{
}
