/*
	@file	Floor.cpp
	@brief	床クラス
*/
#include "pch.h"
#include "Floor.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"

//---------------------------------------------------------
// コンストラクト
//---------------------------------------------------------
Floor::Floor()
	:
	m_commonResources{},
	m_position{DirectX::SimpleMath::Vector3::Zero}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Floor::~Floor()
{
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void Floor::Initialize(CommonResources* resources)
{
	assert(resources);

	// 共通リソースの設定
	m_commonResources = resources;
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	// テクスチャのロード
	DirectX::CreateWICTextureFromFile(
		device,
		ResourceManager::GetTexturePath("Floor").c_str(), 
		nullptr,
		m_Texture.GetAddressOf()
	);
	// ボックスを作成（サイズ：幅、深さ、非常に薄い高さ）
	DirectX::XMFLOAT3 size(FLOORWIDTH, 1.0f,FLOORDEPT); 
	// 立方体の作成
	m_floorPrimitive = DirectX::GeometricPrimitive::CreateBox(context, size);
	// バウンディングボックスの作成
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = size;
}




//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void Floor::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	using namespace DirectX;

	// ワールド行列（床の位置）
	SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::CreateTranslation(m_position);
	// ジオメトリックプリミティブの描画
	m_floorPrimitive->Draw(worldMatrix, view, proj, Colors::White, m_Texture.Get());
}


//---------------------------------------------------------
// 処理
//---------------------------------------------------------
void Floor::Finalize()
{

}
