/*
  @file Vase.cpp
  @brief プレイヤクラス
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Vase/Vase.h"
#include"Game/Scene/PlayScene.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/InputManager.h"
#include <cassert>
#include <algorithm>
#include"Libraries/MyLib/CollisionDebugRenderer.h"

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
	m_modelScale{ 0.025f },
	m_exist{true}
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

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// 座標を
	m_boundingBox.Center = m_position;
	m_boundingBox.Center.y = m_position.y +1.0f;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(1.0f,1.0f,1.0f);

	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = 0.0f;

	//// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//モデルをロードする
	m_vaseModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/vase.cmo", *fx);
	
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
	
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

void Vase::BreacVase()
{
	m_boundingSphere.Radius +=1.0f;

	if (m_boundingSphere.Radius >= 15.0f)
	{
		m_boundingSphere.Radius = 15.0f;
	}
}
