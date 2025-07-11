/*
  @file  BomStay.cpp
  @brief ボムの真下に置くクラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include <Game/ResourceManager/ResourceManager.h>
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomStay.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Bom/State/BomMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Wall/Wall.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
BomStay::BomStay(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	: m_player(playerState)
	, m_commonResources(nullptr)
	, m_bomState(bomState)
	, m_wall(wall)
	, m_position{}
	, m_velocity{}
	, m_gravity(0.0f, BomState::GRAVITY, 0.0f)
	, m_boundingSphere{} 
	, m_bomModel(nullptr)
{
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
BomStay::~BomStay()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void BomStay::Initialize(CommonResources* resources)
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
	m_boundingSphere.Radius = BomState::BOUNDINGSPHERERADIUS;

}

// 事前更新する
void BomStay::PreUpdate()
{
	//	置く状態に変更する
	m_bomState->SetBomPresent(STAY);
	// 現在の位置を取得する
	m_position = m_bomState->GetPosition();
	//爆発するまでの時間
	if(m_bomState->GetExplosionTimer() < 0.0f)m_bomState->SetExplosionTimer(5.0f);
	
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void BomStay::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 重力の更新
	m_velocity += m_gravity * elapsedTime;   
	// 位置の更新
	m_position += m_velocity * elapsedTime;

	//壁の当たり判定
	for (const auto& wall : m_wall){
		HitCheck(wall->GetBoundingBox(),wall->GetExist());
	}
	
	// 地面にめり込まないようにする
	if (m_position.y < BomMovement::GROUNDHEIGHT) m_position.y = BomMovement::GROUNDHEIGHT;

	//　当たり判定を調整する
	m_boundingSphere.Center = m_position;
	//時間が着たら爆発へ移行させる
	if (m_bomState->GetExplosionTimer() < 0.0f){
		// 爆発状態に移行
		m_bomState->ChangeState(m_bomState->GetBomExplosion());
	}
}

// 事後更新する
void BomStay::PostUpdate()
{
	// 位置情報の更新
	m_bomState->SetPosition(m_position);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void BomStay::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();
	
	// ワールド行列を更新する
	Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.005) * 
		Matrix::CreateTranslation(m_position);
	// モデルを表示する
	m_bomModel->Draw(context, *states, world, view, projection);

}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void BomStay::Finalize()
{
}

//---------------------------------------------------------
// 衝突判定する
//---------------------------------------------------------
void BomStay::HitCheck(DirectX::BoundingBox boundingBox, const bool IsWall)
{
	using namespace DirectX::SimpleMath;

	// バウンディングスフィアと壁のAABBが衝突しているかをチェック
	bool isHitWall = m_boundingSphere.Intersects(boundingBox);
	// 反応してないなら飛ばす
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

	// 衝突後の位置調整（位置を少し押し戻す）
	m_position -= normal * 0.01f; // 適宜調整
	
	m_velocity = Vector3(0.0f,0.0f,0.0f);

}