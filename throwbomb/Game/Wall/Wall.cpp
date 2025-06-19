/*
  @file  Wall.cpp
  @brief 壁クラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Wall/Wall.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"

float Wall::THRESHOLD = 0.00f; 

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Wall::Wall(WallType wallType, std::shared_ptr<DirectX::Model> model, const DirectX::SimpleMath::Vector3& position)
	:
	m_commonResources{ nullptr },
	m_collisionDebugRenderer{ nullptr },
	m_depthStencilState{ nullptr },
	m_texture{ nullptr },
	m_wallType{ wallType },
	m_position{ position },
	m_boundingBox{},
	m_scale{ 1.0f },
	m_modelScale{ 0.01f },	//１００分の１の大きさ
	m_exist{ true },
	m_isVisible{ false },
	m_wallModel{ std::move(model) }
{
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Wall::~Wall()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void Wall::Initialize(CommonResources* resources)
{

	assert(resources);  // nullポインタでないか確認
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// バウンディングボックスの設定
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(m_scale);

	// 深度ステンシル設定を構築
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = true;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// ステンシルの設定: 正面の条件と動作
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace = desc.FrontFace;  // 裏面も同じ設定

	// ステンシル状態を作成
	DX::ThrowIfFailed(device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf()));
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void Wall::Render(ID3D11DeviceContext* context,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	
	// 深度ステンシルステート設定
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// ワールド行列の作成
	Matrix world = Matrix::CreateScale(m_modelScale) * Matrix::CreateTranslation(m_position) ;

	// モデルの描画
	switch (m_wallType)
	{
	//破壊不可の壁の描画
	case WallType::Indestructible:
		m_wallModel->Draw(context, *m_commonResources->GetCommonStates(), world, view, projection);
		break;
	//破壊可能の壁の描画
	case WallType::Destructible:
		m_wallModel->Draw(context, *m_commonResources->GetCommonStates(), world, view, projection);
		break;
	default:
		break;
	}
	
#ifdef _DEBUG
	const Vector4 color(DirectX::Colors::Red.f[0], DirectX::Colors::Red.f[1], DirectX::Colors::Red.f[2], DirectX::Colors::Red.f[3]);
	m_collisionDebugRenderer->RenderBoundingBox(m_boundingBox, view, projection, color);
#endif
	
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void Wall::Finalize()
{
	// 深度ステンシルステートの解放
	if (m_depthStencilState) {
		m_depthStencilState.Reset();
	}
}

//---------------------------------------------------------
// 壁との当たり判定
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 Wall::CheckHitAndResolve(const DirectX::BoundingBox& a, const DirectX::BoundingBox& b)
{
	using namespace DirectX::SimpleMath;

	// バウンディングボックス同士の衝突判定をとる || 消えているなら、衝突なし
	if (!a.Intersects(b) || m_isVisible) { return Vector3::Zero; }

	// AABB用のmin/maxを計算する
	Vector3 aMin = a.Center - a.Extents;
	Vector3 aMax = a.Center + a.Extents;
	Vector3 bMin = b.Center - b.Extents;
	Vector3 bMax = b.Center + b.Extents;

	// 各軸の差分を計算する
	float dx1 = bMax.x - aMin.x;
	float dx2 = bMin.x - aMax.x;
	float dy1 = bMax.y - aMin.y;
	float dy2 = bMin.y - aMax.y;
	float dz1 = bMax.z - aMin.z;
	float dz2 = bMin.z - aMax.z;

	// 各軸について、絶対値の小さい方を軸のめり込み量とする：AABBの重なった部分を特定する
	float dx = std::abs(dx1) < std::abs(dx2) ? dx1 : dx2;
	float dy = std::abs(dy1) < std::abs(dy2) ? dy1 : dy2;
	float dz = std::abs(dz1) < std::abs(dz2) ? dz1 : dz2;

	// 衝突時の押し戻しベクトル
	Vector3 pushBackVec = Vector3::Zero;

	// 衝突閾値を超えた場合のみ押し戻し
	if (std::abs(dx) > THRESHOLD || std::abs(dy) > THRESHOLD || std::abs(dz) > THRESHOLD)
	{
		// まずX軸方向での処理
		if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz))
		{
			pushBackVec.x = dx; // X軸方向で押し戻し
		}
		// 次にZ軸方向での処理
		else if (std::abs(dz) <= std::abs(dy))
		{
			pushBackVec.z = dz; // Z軸方向で押し戻し
		}
	}

	pushBackVec.y = dy;

	if (dy < 0.0f)
	{
		pushBackVec.y = 0.0f;
	}

	// 位置が小刻みに振れる問題を防ぐため、小さな修正量を無視
	if (std::abs(pushBackVec.x) < 0.10f) { pushBackVec.x = 0.0f; }
	///*if (std::abs(pushBackVec.y) < 0.10f) { pushBackVec.y = 0.0f; }*/
	if (std::abs(pushBackVec.z) < 0.10f) { pushBackVec.z = 0.0f; }

	// めり込みの差の押し返す
	return pushBackVec;  
}

