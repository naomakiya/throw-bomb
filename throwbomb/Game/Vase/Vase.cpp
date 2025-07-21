/*
  @file Vase.cpp
  @brief プレイヤクラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/Vase/Vase.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Vase::Vase(DirectX::SimpleMath::Vector3 pos)
	:
	m_commonResources{ nullptr },
	m_vaseModel{ nullptr },
	m_collisionDebugRenderer{ nullptr },
	m_position{pos},
	m_boundingBox{},
	m_boundingSphere{},
	m_isExist{true}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Vase::~Vase()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void Vase::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// 当たり判定の座標更新
	m_boundingBox.Center = m_position;
	// 底に座標が有るために上げる
	m_boundingBox.Center.y = m_position.y +1.0f;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(1.0f,1.0f,1.0f);
	// バウディングスフィアの作成
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = 0.0f;

	//// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//モデルをロードする
	m_vaseModel = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("Vase").c_str(), *fx);
	
#ifdef _DEBUG
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	// デバック用の当たり判定
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
#endif
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void Vase::Render(ID3D11DeviceContext* context,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();

	// ワールド行列を更新する
	Matrix world = Matrix::CreateScale(0.025f) * Matrix::CreateTranslation(m_position);
	// モデルを表示する
	m_vaseModel->Draw(context, *states, world, view, projection);

#ifdef _DEBUG
	const Vector4 color(DirectX::Colors::Red.f[0], DirectX::Colors::Red.f[1], DirectX::Colors::Red.f[2], DirectX::Colors::Red.f[3]);
	m_collisionDebugRenderer->RenderBoundingBox(m_boundingBox, view, projection, color);
	
	m_collisionDebugRenderer->RenderBoundingSphere(m_boundingSphere, view, projection, color);
#endif
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void Vase::Finalize()
{
	m_vaseModel.reset();
}

//---------------------------------------------------------
// 壺が破壊されたなら
//---------------------------------------------------------
void Vase::BreacVase()
{
	// スフィアの大きさを広げる
	m_boundingSphere.Radius +=1.0f;
	// 行って以上の大きさになったらそれ以上おおきくならないようにする
	if (m_boundingSphere.Radius >= 15.0f){
		m_boundingSphere.Radius = 15.0f;
	}
}
