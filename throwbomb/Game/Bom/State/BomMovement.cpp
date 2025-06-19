/*
  @file BomMovement.cpp
  @brief ボム移動状態クラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/Resources.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Wall/Wall.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Libraries/MyLib/DebugString.h"

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
	,m_gravity(0.0f, GravityY, 0.0f)
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
	//auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");

	//モデルをロードする
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Bom").c_str(), *fx);

	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = BoundingSphereRadius;

#ifdef _DEBUG	// デバック時実行
	//デバック用のコリジョン
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
#endif
}

// 事前更新する
void BomMovement::PreUpdate()
{
	m_position = m_bomState->GetPosition();
	m_bomState->SetExplosionTimer(CountdownTime);
	Projection(m_player->GetForwardDirection());
	m_bomState->SetVelocity(m_velocity);
	m_bomState->SetBounce(false);
	m_bomState->SetBomPresent(BomState::BomtPresent::MOVEMENT);
	
}
//---------------------------------------------------------
//投射投げ
//---------------------------------------------------------
void BomMovement::Projection(const DirectX::SimpleMath::Vector3& playerForwardDirection)
{
	// 爆弾の初速度の設定（角度を決める）
	float angleDeg = m_bomState->GetAngle();                    
	// ラジアンに変換
	float angleRad = DirectX::XMConvertToRadians(angleDeg);

	// 初速度ベクトルの計算（水平方向と垂直方向の成分）
	float vx = LaunchSpeed * std::cos(angleRad);           // 水平方向の速度成分
	float vy = LaunchSpeed * std::sin(angleRad);           // 垂直方向の速度成分

	// 初速度ベクトルの設定
	m_velocity = playerForwardDirection * vx + DirectX::SimpleMath::Vector3(0.0f, vy, 0.0f);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void BomMovement::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	m_bomState->SetBoundingSphere(m_boundingSphere);

	m_boundingSphere.Center = m_position;
	m_velocity = m_bomState->GetVelocity();
	m_velocity += m_gravity * elapsedTime;
	
	// 地面に着地した場合、Y方向の速度を反転させる
	if (m_position.y < GroundHeight)
	{
		m_bomState->SetBounce(true);
		m_velocity = DirectX::SimpleMath::Vector3::Zero;
	}
	//バウンスしているか
	if (m_bomState->GetBounce())
	{
		HandleCollision();
	}
	//壁との当たり判定
	for (const auto& wall : m_wall)
	{
		CheckHit(wall->GetBoundingBox(),wall->GetExist());
	}

	// 位置の更新
	m_position += m_velocity * elapsedTime;

	//0より下にいかないように
	if (m_position.y < MinHeight)
	{
		m_position.y = MinHeight;
	}
	//時間が来たら爆発させる
	if (m_bomState->GetExplosionTimer() < 0.0f)
	{
		m_bomState->ChangeState(m_bomState->GetBomExplosion());
	}

	m_boundingSphere.Center = m_position;
	m_bomState->SetPosition(m_position);
	m_bomState->SetVelocity(m_velocity);
}

// 事後更新する
void BomMovement::PostUpdate()
{
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
	Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.005f) * 
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
	m_velocity = -m_velocity * BounceFactor;

	m_bomState->SetBounce(false);
}

//---------------------------------------------------------
// 衝突判定する
//---------------------------------------------------------
void BomMovement::CheckHit(DirectX::BoundingBox boundingBox, const bool IsWall)
{
	using namespace DirectX::SimpleMath;

	// バウンディングスフィアと壁のAABBが衝突しているかをチェック
	bool isHitWall = m_boundingSphere.Intersects(boundingBox);

	if (!isHitWall || !IsWall ) { return; }

	// 衝突時の速度ベクトルを計算する
	Vector3 bMin = boundingBox.Center - boundingBox.Extents;
	Vector3 bMax = boundingBox.Center + boundingBox.Extents;

	Vector3 normal = Vector3::Zero;

	if (m_position.x < bMin.x || m_position.x > bMax.x) // X方向の衝突
	{
		normal.x = (m_position.x < bMin.x) ? 1.0f : -1.0f;
	}
	else if (m_position.y < bMin.y || m_position.y > bMax.y) // Y方向の衝突
	{
		normal.y = (m_position.y < bMin.y) ? 1.0f : -1.0f;
	}
	else if (m_position.z < bMin.z || m_position.z > bMax.z) // Z方向の衝突
	{
		normal.z = (m_position.z < bMin.z) ? 1.0f : -1.0f;
	}

	// 反射ベクトルを計算する
	Vector3 reflection = m_velocity - 2.0f * (m_velocity.Dot(normal)) * normal;

	// 反射ベクトルを設定する
	m_velocity = reflection * BounceFactor;

	// 衝突後の位置調整（位置を少し押し戻す）
	m_position -= normal * 0.01f; // 適宜調整

}