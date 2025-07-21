/*
  @file BomMovement.cpp
  @brief ボム移動状態クラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Libraries/MyLib/DebugString.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Wall/Wall.h"



//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
BomMovement::BomMovement(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	:m_commonResources(nullptr)
	,m_player(playerState)
	,m_bomState(bomState)
	,m_wall(wall)
	,m_position{}
	,m_velocity{}
	,m_gravity(0.0f, BomState::GRAVITY, 0.0f)
	,m_isBounce(false)
{
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
BomMovement::~BomMovement()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void BomMovement::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	
	// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//モデルをロードする
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("Bom").c_str(), *fx);
	// バウディングスフィアの設定
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = BomState::BOUNDINGSPHERERADIUS;

#ifdef _DEBUG	// デバック時実行
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	//デバック用のコリジョン
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
#endif
}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void BomMovement::PreUpdate()
{
	//現在の位置を取得
	m_position = m_bomState->GetPosition();
	// カウントダウンの時間を設定
	m_bomState->SetExplosionTimer(COUNTDOWNTIME);
	// 現在向いている方向の取得
	Projection(m_player->GetForwardDirection());
	// 速度の設定
	m_bomState->SetVelocity(m_velocity);
	// バウンスをしていないに設定
	m_bomState->SetBounce(false);
	// 現在の状態を移動にする
	m_bomState->SetBomPresent(BomState::BomtPresent::MOVEMENT);
}

//---------------------------------------------------------
// 投射投げ
//---------------------------------------------------------
void BomMovement::Projection(const DirectX::SimpleMath::Vector3& playerForwardDirection)
{
	// 爆弾の初速度の設定（角度を決める）
	float angleDeg = m_bomState->GetAngle();                    
	// ラジアンに変換
	float angleRad = DirectX::XMConvertToRadians(angleDeg);
	// 初速度ベクトルの計算
	float vx = LAUNCHSPEED * std::cos(angleRad);           // 水平方向の速度成分
	float vy = LAUNCHSPEED * std::sin(angleRad);           // 垂直方向の速度成分
	// 初速度ベクトルの設定
	m_velocity = playerForwardDirection * vx + DirectX::SimpleMath::Vector3(0.0f, vy, 0.0f);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void BomMovement::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	// バウディングスフィアの設定
	m_bomState->SetBoundingSphere(m_boundingSphere);
	// 当たり判定の位置の更新
	m_boundingSphere.Center = m_position;
	// 一定速度の上書き
	m_velocity = m_bomState->GetVelocity();
	// 重力の更新
	m_velocity += m_gravity * elapsedTime;
	// 地面に着地した場合、Y方向の速度を反転させる
	if (m_position.y < GROUNDHEIGHT){
		// バウンス状態にする
		m_bomState->SetBounce(true);
		// 速度をゼロにする
		m_velocity = DirectX::SimpleMath::Vector3::Zero;
	}
	//バウンスしているか
	if (m_bomState->GetBounce()){
		// バウンス処理をする
		HandleCollision();
	}
	//壁との当たり判定
	for (const auto& wall : m_wall){
		HitCheck(wall->GetBoundingBox(),wall->GetExist());
	}
	// 位置の更新
	m_position += m_velocity * elapsedTime;
	//0より下にいかないように
	if (m_position.y < MINHEIGHT){
		m_position.y = MINHEIGHT;
	}
	//時間が来たら爆発させる
	if (m_bomState->GetExplosionTimer() < 0.0f){
		// 爆発状態に移行
		m_bomState->ChangeState(m_bomState->GetBomExplosion());
	}
	//　バウディングスフィアの位置を更新
	m_boundingSphere.Center = m_position;
	// 位置を更新する
	m_bomState->SetPosition(m_position);
	// 速度の更新
	m_bomState->SetVelocity(m_velocity);
}

// 事後更新する
void BomMovement::PostUpdate()
{
	//位置の更新
	m_bomState->SetPosition(m_position);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void BomMovement::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();
	
	// ワールド行列を更新する
	Matrix world = DirectX::SimpleMath::Matrix::CreateScale(BomState::BOMBMODELSIZE) * 
		Matrix::CreateTranslation(m_position);
	// モデルを表示する
	m_bomModel->Draw(context, *states, world, view, projection);

#ifdef _DEBUG
	// デバッグ情報を「DebugString」で表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("");
	debugString->AddString("BX : %f", m_position.x);
	debugString->AddString("BY : %f", m_position.y);
	debugString->AddString("BZ : %f", m_position.z);
	
	//コリジョンの色
	const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
		DirectX::Colors::Red.f[1],
		DirectX::Colors::Red.f[2],
		DirectX::Colors::Red.f[3]);
	//デバッグ描画
	m_collisionDebugRenderer->RenderBoundingSphere(m_boundingSphere, view, projection, color);
#endif

}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void BomMovement::Finalize()
{
}

//---------------------------------------------------------
//ボムがバウンドする
//---------------------------------------------------------
void BomMovement::HandleCollision()
{
	// 反射ベクトルを設定する
	m_velocity = -m_velocity * BOUNCEFACTOR;
	// バウンスしていない状態にする
	m_bomState->SetBounce(false);
}

//---------------------------------------------------------
// 壁との衝突判定する
//---------------------------------------------------------
void BomMovement::HitCheck(DirectX::BoundingBox boundingBox, const bool IsWall)
{
	using namespace DirectX::SimpleMath;

	// バウンディングスフィアと壁のAABBが衝突しているかをチェック
	bool isHitWall = m_boundingSphere.Intersects(boundingBox);
	// していないなら飛ばす
	if (!isHitWall || !IsWall ) { return; }
	// 衝突時の速度ベクトルを計算する
	Vector3 bMin = boundingBox.Center - boundingBox.Extents;
	Vector3 bMax = boundingBox.Center + boundingBox.Extents;
	// 押し返しの数値を入れるための物
	Vector3 normal = Vector3::Zero;
	// X方向の衝突
	if (m_position.x < bMin.x || m_position.x > bMax.x) {
		normal.x = (m_position.x < bMin.x) ? 1.0f : -1.0f;
	}
	// Y方向の衝突
	else if (m_position.y < bMin.y || m_position.y > bMax.y){
		normal.y = (m_position.y < bMin.y) ? 1.0f : -1.0f;
	}
	// Z方向の衝突
	else if (m_position.z < bMin.z || m_position.z > bMax.z){
		normal.z = (m_position.z < bMin.z) ? 1.0f : -1.0f;
	}
	// 反射ベクトルを計算する(強く反射させるために２倍）
	Vector3 reflection = m_velocity - 2.0f * (m_velocity.Dot(normal)) * normal;
	// 反射ベクトルを設定する
	m_velocity = reflection * BOUNCEFACTOR;
	// 衝突後の位置調整（位置を少し押し戻す）
	m_position -= normal * 0.01f; // これしないとめっちゃ不審な動きする

}