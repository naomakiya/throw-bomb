/*
  @file BomExist.cpp
  @brief ボム生存状態クラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomExist.h"
#include "Game/Bom/State/BomMovement.h"
#include "Game/Bom/State/BomStay.h"
#include "Game/Wall/Wall.h"
#include "Game/Player/PlayerState.h"


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
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("Bom").c_str(), *fx);

}

//---------------------------------------------------------
// 事前更新する
//---------------------------------------------------------
void BomExist::PreUpdate()
{
	//生存を無くす
	m_isExist = false;
	// 角度を初期化する
	m_currentAngle = 0.0f;
	// 現在の状態に変更する
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
	if (!m_isExist){
		m_isExist = m_bom->GetExist();
		return;
	}

	// プレイヤーの位置取得
	m_position = m_player->GetPosition();
	// プレイヤーの上に置く
	m_position.y += BOMSPAWNHEIGHT;
	// 情報を更新する
	m_bom->SetPosition(m_position);
	// 保持をする
	m_isHoldingBom = true;

	// 投げ準備スペースキーを押した瞬間
	if (kbTracker->pressed.Space && m_isHoldingBom){
		// spaceキーを長押しする
		m_isSpaceHeld = true;
		// 長押し時間をリセット
		m_holdStartTime = 0.0f;
	}

	// 投げ実行スペースキー離した瞬間
	if (kbTracker->released.Space && m_isHoldingBom){
		// ボムを投げる状態に変更
		m_bom->ChangeState(m_bom->GetBomMovement());
		return;
	}

	// 置く処理Xキーでその場に設置
	if (kbTracker->pressed.X && m_isHoldingBom){
		// ボムをその場に置く状態にする
		m_bom->ChangeState(m_bom->GetBomStay());
		return;
	}

	// スペースキー長押し処理角度スイング＋強制投げ判定
	if (m_isSpaceHeld){
		// 時間経過
		m_holdStartTime += elapsedTime;

		// 角度スイング処理
		if (m_isAngleIncreasing){
			// 角度の変更
			m_currentAngle -= elapsedTime * ANGLESPEED;
			// 上昇させる（大きくする）
			if (m_currentAngle <= 0.0f) m_isAngleIncreasing = false;
		}
		else{
			// 角度の変更
			m_currentAngle += elapsedTime * ANGLESPEED;
			// 下昇させる（小さくする）
			if (m_currentAngle >= 90.0f) m_isAngleIncreasing = true;
		}

		// 長押しが10秒超えたら強制投げ
		if (m_holdStartTime >= 10.0f){
			// ホールド保持を無しにする
			m_isSpaceHeld = false;
			// 移動状態に移行する
			m_bom->ChangeState(m_bom->GetBomMovement());
			return;
		}
	}
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void BomExist::PostUpdate()
{
	// 保持をしない
	m_isSpaceHeld = false;
	// 角度をセットする
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
		DirectX::SimpleMath::Matrix::CreateScale(BomState::BOMBMODELSIZE) *
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
