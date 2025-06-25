/*
  @file  PlayerIdling.cpp
  @brief プレイヤー待機状態クラス
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Player/State/PlayerIdling.h"
#include"Game/Player/PlayerState.h"
#include"Game/Player/State/PlayerBomHand.h"
#include"Game/Player/State/PlayerMovement.h"
#include"Game/Bom/BomState.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayerIdling::PlayerIdling(PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_player(playerState),
	m_totalSeconds(0.0f),
	m_playerface{},
	m_playerfaceBody{},
	m_position{},
	m_rotate{},
	m_commonResources{},
	m_breathingFactor(0.0f),
	m_playerHandL{},
	m_playerHandR{},
	m_time{ 0 },
	m_hand{},
	m_wall{ wall },
	m_modelScale{0.008f}
{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
PlayerIdling::~PlayerIdling()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void PlayerIdling::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/Player");

	//モデルをロードする
	m_playerface = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/face.cmo", *fx);
	m_playerfaceBody = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/Body.cmo", *fx);
	m_playerHandL = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/PlayerHand.cmo", *fx);
	m_playerHandR = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/PlayerHand.cmo", *fx);

	PreUpdate();
}

// 事前更新する
void PlayerIdling::PreUpdate()
{
	m_position = m_player->GetPosition();
	m_rotate = m_player->GetAngle();

}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayerIdling::Update(const float& elapsedTime)
{
	using namespace DirectX::SimpleMath;

	UNREFERENCED_PARAMETER(elapsedTime);

	//事前に必要な情報を獲得する
	this->PreUpdate();
	//時間経過
	m_time += elapsedTime;
	// キーボードステートを取得する
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();
	//矢印キーを押したら移動状態に変える
	if (keyboardState.Up || keyboardState.Down || keyboardState.Left || keyboardState.Right)
	{
		m_player->ChangeState(m_player->GetPlayerMovement());
	}

}

// 事後更新する
void PlayerIdling::PostUpdate()
{
	m_player->SetPosition(m_position);
	m_player->SetAngle(m_rotate);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayerIdling::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// 顔の揺れを計算
	float faceSwayAngle = std::sin(m_time * 1.5f) * 0.35f;
	Quaternion faceRotation = Quaternion::CreateFromAxisAngle(Vector3::Up, faceSwayAngle);

	// オフセットの定義
	Vector3 handBaseOffsetL(0.7f, m_hand, 0.1f);
	Vector3 handBaseOffsetR(-0.7f, m_hand, 0.1f);

	Vector3 legOffsetL(0.5f, -0.5f, 0.0f);
	Vector3 legOffsetR(-0.5f, -0.5f, 0.0f);

	// プレイヤーの回転に応じてオフセットを変換
	float handAngle = -sin(m_time * 1.5f) * 0.25f; // 回転速度を調整
	float handRadius = 0.7f; // 円の半径

	Vector3 handOffsetL(handRadius * std::cos(handAngle), 0, handRadius * std::sin(handAngle));
	Vector3 handOffsetR(handRadius * std::cos(handAngle + DirectX::XM_PI), 0.0f, handRadius * std::sin(handAngle + DirectX::XM_PI));

	// プレイヤーの回転に応じてオフセットを変換
	handOffsetL = Vector3::Transform(handOffsetL, m_rotate);
	handOffsetR = Vector3::Transform(handOffsetR, m_rotate);
	legOffsetL = Vector3::Transform(legOffsetL, m_rotate);
	legOffsetR = Vector3::Transform(legOffsetR, m_rotate);

	// ワールド行列の計算
	Matrix faceWorld = Matrix::CreateScale(m_modelScale) * Matrix::CreateFromQuaternion(m_rotate * faceRotation) * Matrix::CreateTranslation(m_position);
	Matrix bodyWorld = Matrix::CreateScale(m_modelScale) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_position);

	Matrix handScale = Matrix::CreateScale(m_modelScale);
	Matrix handRotation = Matrix::CreateFromQuaternion(faceRotation * m_position);

	Matrix handL = handScale * handRotation * Matrix::CreateTranslation(m_position + handOffsetL);
	Matrix handR = handScale * handRotation * Matrix::CreateTranslation(m_position + handOffsetR);

	Matrix legL = handScale * Matrix::CreateTranslation(m_position + legOffsetL);
	Matrix legR = handScale * Matrix::CreateTranslation(m_position + legOffsetR);

	if (!m_player->GetHitEnemy())
	{
		// モデルの描画
		m_playerface->Draw(context, *states, faceWorld, view, projection);
		m_playerfaceBody->Draw(context, *states, bodyWorld, view, projection);
		m_playerHandL->Draw(context, *states, handL, view, projection);
		m_playerHandR->Draw(context, *states, handR, view, projection);
		m_playerHandL->Draw(context, *states, legL, view, projection);
		m_playerHandR->Draw(context, *states, legR, view, projection);
	}
	else
	{
		m_player->EnemyHitFlashing(*m_playerface, faceWorld, view, projection);
		m_player->EnemyHitFlashing(*m_playerfaceBody, bodyWorld, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandL, handL, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandR, handR, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandL, legL, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandR, legR, view, projection);
	}

}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void PlayerIdling::Finalize()
{
}
