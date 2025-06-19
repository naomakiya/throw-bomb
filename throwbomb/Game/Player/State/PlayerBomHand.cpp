/*
  @file PlayerBomHand.cpp
  @brief プレイヤボム持ちクラス
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/Player/State/PlayerBomHand.h"
#include "Game/Player/State/PlayerIdling.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/DebugString.h"
#include "Game/Wall/Wall.h"
#include "Game/Camera/Camera.h"
#include "Game/Bom/BomState.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayerBomHand::PlayerBomHand(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	: m_commonResources(nullptr)
	, m_camera(camera)
	, m_wall(wall)
	, m_player(playerState)
	, m_position(DirectX::SimpleMath::Vector3::Zero)
	, m_rotate(DirectX::SimpleMath::Quaternion::Identity)
	, m_handHeight(0.0f)
	, m_time(0.0f)
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
PlayerBomHand::~PlayerBomHand()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void PlayerBomHand::Initialize(CommonResources* resources)
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

}

//---------------------------------------------------------
// 事前更新する
//---------------------------------------------------------
void PlayerBomHand::PreUpdate()
{
	m_position = m_player->GetPosition();
	m_rotate = m_player->GetAngle();
	
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayerBomHand::Update(const float& elapsedTime)
{
	using namespace DirectX::SimpleMath;
	//時間経過
	m_time += elapsedTime;
	// キーボードの状態を取得する
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	Vector3 velocity = Vector3::Zero;
	//現在の位置を獲得する
	m_position = m_player->GetPosition();

	// 移動方向の取得
	if (kbState.Up) {
		velocity += Vector3(0.0f, 0.0f, 25.0f); // 前方
	}
	if (kbState.Down) {
		velocity += Vector3(0.0f, 0.0f, -25.0f); // 後方
	}
	if (kbState.Left) {
		velocity += Vector3(25.0f, 0.0f, 0.0f); // 左
	}
	if (kbState.Right) {
		velocity += Vector3(-25.0f, 0.0f, 0.0f); // 右
	}


	// 移動方向がある場合
	if (velocity.LengthSquared() > 0) 
	{
		velocity.Normalize(); // ベクトルを正規化
		Vector3 vel = { 2.5f,0.0f,2.5f };

		// カメラの回転を取得
		Quaternion cameraRotation = m_camera->GetRotation(); // カメラの回転を取得
		// プレイヤーの移動方向をカメラの向きに変換
		Vector3 moveDirection = Vector3::Transform(velocity, cameraRotation);

		// プレイヤーの向きを更新
		float yaw = atan2f(moveDirection.x, moveDirection.z);
		m_rotate = Quaternion::CreateFromYawPitchRoll(yaw, 0.0f, 0.0f); // プレイヤーを向かせる

		// 移動量を計算
		moveDirection *= vel; // 加速後の速度を移動に適用

		// プレイヤーの位置を更新
		m_position += moveDirection * elapsedTime;
	}

	// プレイヤーの位置を更新
	this->PostUpdate();

}

//---------------------------------------------------------
// 事後更新する
//---------------------------------------------------------
void PlayerBomHand::PostUpdate()
{
	// プレイヤーの位置を更新
	m_player->SetPosition(m_position);
	// プレイヤーの向きを更新
	m_player->SetAngle(m_rotate);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayerBomHand::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();
	
	m_handHeight = 0.75f;
	// 顔の揺れを計算
	float faceSwayAngle = std::sin(m_time * 1.5f) * 0.35f;
	Quaternion faceRotation = Quaternion::CreateFromAxisAngle(Vector3::Up, faceSwayAngle);

	// プレイヤーの向きに対して左右に手を広げるためのベクトルを定義
	Vector3 handOffsetL = Vector3(0.35f, 0.0f, 0.0f);  // 左手のオフセット
	Vector3 handOffsetR = Vector3(-0.35f, 0.0f, 0.0f); // 右手のオフセット
	Vector3 legOffsetL(0.5f, -0.5f, 0.0f);
	Vector3 legOffsetR(-0.5f, -0.5f, 0.0f);

	// 手の向きをプレイヤーの回転に平行にする
	handOffsetL = Vector3::Transform(handOffsetL, m_rotate);  // プレイヤーの回転に応じた左手のオフセット
	handOffsetR = Vector3::Transform(handOffsetR, m_rotate);  // プレイヤーの回転に応じた右手のオフセット
	legOffsetL = Vector3::Transform(legOffsetL, m_rotate);
	legOffsetR = Vector3::Transform(legOffsetR, m_rotate);

	// ワールド行列の計算
	Matrix faceWorld = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate * faceRotation) * Matrix::CreateTranslation(m_position);
	Matrix bodyWorld = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_position);

	// 手のワールド行列を作成
	Matrix handL = Matrix::CreateScale(m_player->PLAYERMODLSCALE)
		* Matrix::CreateTranslation(m_position + handOffsetL + Vector3(0.0f, m_handHeight, 0.0f)); // 左手の位置
	Matrix handR = Matrix::CreateScale(m_player->PLAYERMODLSCALE)
		* Matrix::CreateTranslation(m_position + handOffsetR + Vector3(0.0f, m_handHeight, 0.0f)); // 右手の位置


	//足のワールド行列
	Matrix legL = Matrix::CreateScale(0.0085f) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_position + legOffsetL);
	Matrix legR = Matrix::CreateScale(0.0085f) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_position + legOffsetR);

	if(!m_player->GetHitEnemy())
	{
		// モデルを表示する
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
		m_player->EnemyHitFlashing(*m_playerfaceBody, bodyWorld,view,projection);
	}

}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void PlayerBomHand::Finalize()
{
}

//---------------------------------------------------------
// 移動
//---------------------------------------------------------
void PlayerBomHand::Movement(const float& elapsedTime)
{
	using namespace DirectX::SimpleMath;
	// キーボードステートを取得する
	DirectX::Keyboard::State kbState = DirectX::Keyboard::Get().GetState();
	// 速さ
	Vector3 velocity = Vector3::Zero;

	// ロール、ピッチ、ヨー
	float roll = 0.0f;	// Ｚ軸回転
	float pitch = 0.0f;	// Ｘ軸回転
	float yaw = 0.0f;	// Ｙ軸回転

	if (kbState.Left)	yaw = DirectX::XMConvertToRadians(1.0f);
	if (kbState.Right)	yaw = DirectX::XMConvertToRadians(-1.0f);

	// クォータニオンをロール、ピッチ、ヨーの値から生成する
	Quaternion q = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	// 回転
	m_rotate = q * m_rotate;
	// 正規化する
	velocity.Normalize();

	// 移動量を計算する
	velocity *= m_player->SPEED;;

	// プレイヤーの位置を更新する
	m_position += velocity * elapsedTime;

	// プレイヤーの移動を試みる
	Vector3 newPosition = m_position + velocity;

	// プレイヤーの位置を更新する
	m_position = newPosition;
	// 座標を更新する
	if (kbState.Up)		m_position += Vector3::Transform(Vector3(0.0f, 0.0f, 0.1f), m_rotate);
	if (kbState.Down)	m_position -= Vector3::Transform(Vector3(0.0f, 0.0f, 0.1f), m_rotate);
}
