/*
  @file Player.cpp
  @brief プレイヤクラス
*/
#include "pch.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemySearch.h"
#include"Game/Enemy/StraighteningEnemyState.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemyDash.h"
#include"Framework/CommonResources.h"
#include "Framework/Resources.h"
#include"Game/Wall/Wall.h"
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <DirectXCollision.h>
#include"Interface/IEnemyState.h"
#include"Game/Player/PlayerState.h"
#include"Libraries/MyLib/CollisionDebugRenderer.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
 StraighteningEnemySearch:: StraighteningEnemySearch(StraighteningEnemyState* enemyState,
    const std::vector<std::unique_ptr<Wall>>& wall,PlayerState* player )
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
	m_rotation{ DirectX::SimpleMath::Quaternion::Identity },
	m_player{player},
	m_collisionDebugRenderer{},
	m_searchBoundingBox{}
{

}

 StraighteningEnemySearch::~ StraighteningEnemySearch()
{
}

void  StraighteningEnemySearch::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = pos;
	m_position.z = pos.z + 0.5f; //ボックスの位置を前に0.5f移動させるする

	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
	
	// バウンディングボックスのサイズを設定（幅2.0、高さ2.0、奥行き2.0の立方体）
	m_searchBoundingBox = DirectX::BoundingOrientedBox(pos, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.5f), DirectX::SimpleMath::Quaternion::Identity);
}


void  StraighteningEnemySearch::PreUpdate()
{
	m_position = m_enemy->GetPosition();
	m_boundingBox = m_enemy->GetBoundingBox();
	m_boundingBox.Center = m_position;
}

void  StraighteningEnemySearch::Update(const float& elapsedTime)
{
	this->PreUpdate();
	// 1秒で15度回転する
	float rotationSpeed = DirectX::XMConvertToRadians(15.0f) * elapsedTime;

	// Y軸回転のクォータニオンを作成し、現在の回転に適用
	DirectX::SimpleMath::Quaternion rotationDelta = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, rotationSpeed);
	m_rotation = m_rotation * rotationDelta;
	m_rotation.Normalize(); // 正規化

	// 敵の前方ベクトルを取得
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Forward;
	forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation));

	// 探索用の箱の更新
	
	// 前方に配置
	m_searchBoundingBox.Center = m_position + forward * -1.5f;  
	// 回転を適用
	m_searchBoundingBox.Orientation = m_rotation; 

	if (m_searchBoundingBox.Intersects(m_player->GetBoundingBox()))
	{
		m_enemy->ChangeState(m_enemy->GetStraighteningEnemyDash());
	}
}

void  StraighteningEnemySearch::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

void  StraighteningEnemySearch::Render(ID3D11DeviceContext* context,
	const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
	auto states = m_commonResources->GetCommonStates();

	// 回転をクォータニオンからマトリクスへ変換
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation);

	// ワールド行列を作成
	DirectX::SimpleMath::Matrix world =
		DirectX::SimpleMath::Matrix::CreateScale(0.006) *
		rotationMatrix *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	//赤色で描画
	const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
		DirectX::Colors::Red.f[1],
		DirectX::Colors::Red.f[2],
		DirectX::Colors::Red.f[3]);

	// モデルを表示する
	model.Draw(context, *states, world, view, projection);

}

	

void  StraighteningEnemySearch::Finalize()
{
}

