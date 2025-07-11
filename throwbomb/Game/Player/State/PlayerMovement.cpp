/*
  @file  PlayerMovement.cpp
  @brief プレイヤ移動状態クラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Player/State/PlayerMovement.h"
#include "Game/Player/State/PlayerBomHand.h"
#include "Game/Player/State/PlayerIdling.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/Wall/Wall.h"
#include "Game/Camera/Camera.h"
#include "Game/Bom/BomState.h"
#include "Game/ResourceManager/ResourceManager.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayerMovement::PlayerMovement(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_postion{},
	m_player{ playerState },
	m_currentState{},
	m_playerHandL{},
	m_playerHandR{},
	m_commonResources{},
	m_wall{ wall },
	m_angle{ 0.0f },
    m_scale{ 0.0f },
	m_rotate{},
	m_camera{camera},
	m_hand{0.0f},
	m_handAngle{0.0f},
	m_time{0.0f},
	m_vel{0}
{
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
PlayerMovement::~PlayerMovement()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void PlayerMovement::Initialize(CommonResources* resources)
{
	m_commonResources = resources;
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	
	// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/Player");
	//モデルをロードする
	m_playerface = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerFace").c_str(), *fx);
	m_playerfaceBody = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerBody").c_str(), *fx);
	m_playerHandL = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerHand").c_str(), *fx);
	m_playerHandR = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerHand").c_str(), *fx);

}

//事前準備
void PlayerMovement::PreUpdate()
{
	// 前の状態の位置情報を取得
	m_postion = m_player->GetPosition();
	// 前の状態の方向情報を取得
	m_rotate = m_player->GetAngle();
	// 速度を消す
	m_vel = 0.0f;
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayerMovement::Update(const float& elapsedTime)
{
	//　位置情報の更新
	m_postion = m_player->GetPosition();
	// 回転速度を調整
	m_handAngle = elapsedTime * 5.0f;
	//移動
	Movement(elapsedTime);
	// プレイヤーの更新
	PostUpdate();
}

//---------------------------------------------------------
// 事前更新する
//---------------------------------------------------------
void PlayerMovement::PostUpdate()
{
	// プレイヤーの位置を更新
	m_player->SetPosition(m_postion);
	m_player->SetAngle(m_rotate);

}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayerMovement::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	//壁になどの触れた後の位置情報を取得
	m_postion = m_player->GetPosition();

	// ワールド行列を更新する
	Matrix world = Matrix::CreateScale(m_player->PLAYERMODLSCALE);
	world *= Matrix::CreateFromQuaternion(m_rotate);
	world *= Matrix::CreateTranslation(m_postion);

	// 顔
	Matrix world2 = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateTranslation(m_postion);

	// 手の振り幅と角度を計算
	float handRadius = 0.7f; // 円の半径

	// 手のオフセット計算（
	Vector3 handOffsetL(handRadius * std::cos(m_handAngle),  handRadius * std::sin(m_handAngle), handRadius * std::cos(m_handAngle));
	Vector3 handOffsetR(handRadius * std::cos(m_handAngle + DirectX::XM_PI), std::max(0.0f, handRadius * std::sin(m_handAngle + DirectX::XM_PI)), 0.1f);

	// 手の向きをプレイヤーの回転に平行にする
	handOffsetL = Vector3::Transform(handOffsetL, m_rotate);
	handOffsetR = Vector3::Transform(handOffsetR, m_rotate);

	// 手のワールド行列を作成
	Matrix handL = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateTranslation(m_postion + handOffsetL);
	Matrix handR = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateTranslation(m_postion + handOffsetR);

	// 足のオフセットとワールド行列計算
	Vector3 legOffsetL = Vector3(0.5f, -0.5f, 0.0f);
	Vector3 legOffsetR = Vector3(-0.5f, -0.5f, 0.0f);

	legOffsetL = Vector3::Transform(legOffsetL, m_rotate);
	legOffsetR = Vector3::Transform(legOffsetR, m_rotate);

	Matrix legL = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_postion + legOffsetL);
	Matrix legR = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_postion + legOffsetR);

	if (!m_player->GetHitEnemy()){
		// モデルを表示する
		m_playerface->Draw(context, *states, world, view, projection);
		m_playerfaceBody->Draw(context, *states, world2, view, projection);
		m_playerHandL->Draw(context, *states, handL, view, projection);
		m_playerHandR->Draw(context, *states, handR, view, projection);
		m_playerHandL->Draw(context, *states, legL, view, projection);
		m_playerHandR->Draw(context, *states, legR, view, projection);
	}
	else{
		// 透明にする
		m_player->EnemyHitFlashing(*m_playerface, world, view, projection);
		m_player->EnemyHitFlashing(*m_playerfaceBody, world2, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandL, handL, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandR, handR, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandL, legL, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandR, legR, view, projection);
	}
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void PlayerMovement::Finalize()
{
}

//---------------------------------------------------------
//移動する
//---------------------------------------------------------
void PlayerMovement::Movement(const float& elapsedTime)
{
	using namespace DirectX::SimpleMath;
	// キーボードの状態を取得する
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	Vector3 velocity = Vector3::Zero;

	// 移動方向の取得
	if (kbState.Up) {
		velocity += Vector3(0.0f, 0.0f, 1.0f); // 前方
	}
	if (kbState.Down) {
		velocity += Vector3(0.0f, 0.0f, -1.0f); // 後方
	}
	if (kbState.Left) {
		velocity += Vector3(1.0f, 0.0f, 0.0f); // 左
	}
	if (kbState.Right) {
		velocity += Vector3(-1.0f, 0.0f, 0.0f); // 右
	}

	// 移動方向がある場合
	if (velocity.LengthSquared() > 0)
	{
		// ベクトルを正規化
		velocity.Normalize(); 
		// 加速を適用（速度の増加）
		m_vel += acceleration; // m_vel（速度）に加速度を加える
		// 最大速度を超えないように制限
		if (m_vel > MAXSPEED) { m_vel = MAXSPEED; }
		// カメラの回転を取得
		Quaternion cameraRotation = m_camera->GetRotation(); 
		// プレイヤーの移動方向をカメラの向きに変換
		Vector3 moveDirection = Vector3::Transform(velocity, cameraRotation);
		// プレイヤーの向きを更新
		float yaw = atan2f(moveDirection.x, moveDirection.z);
		m_rotate = Quaternion::CreateFromYawPitchRoll(yaw, 0.0f, 0.0f); // プレイヤーを向かせる
		// 移動量を計算
		moveDirection *= m_vel ; // 加速後の速度を移動に適用
		// プレイヤーの位置を更新
		m_postion += moveDirection * elapsedTime;
	}

	// プレイヤーが移動していない場合はアイドル状態にする
	if (kbState.Up == false && kbState.Down == false && !kbState.Left && !kbState.Right) {
		m_player->ChangeState(m_player->GetPlayerIdling());
	}
}
