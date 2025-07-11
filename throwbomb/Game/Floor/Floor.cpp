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
	m_position{DirectX::SimpleMath::Vector3(0.0f,0.0f,0.0f)}
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
	using namespace DirectX;

	assert(resources);

	// 共通リソースの設定
	m_commonResources = resources;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// エフェクトの作成
	m_BatchEffect = std::make_unique<AlphaTestEffect>(device);
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_BatchEffect->SetReferenceAlpha(255);

	// 入力レイアウト生成
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_BatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_InputLayout.GetAddressOf()
	);

	// テクスチャのロード
	CreateWICTextureFromFile(
		device,
		ResourceManager::GetTexturePath("Floor").c_str(), 
		nullptr,
		m_Texture.GetAddressOf()
	);
	

	// ボックスを作成（サイズ：幅、深さ、非常に薄い高さ）
	XMFLOAT3 size(FLOORWIDTH, 1.0f,FLOORDEPT); 

	// 立方体の作成
	m_floorPrimitive = GeometricPrimitive::CreateBox(context, size);

	// バウンディングボックスの作成
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = size;

	// 深度ステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.StencilEnable = true;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace = desc.FrontFace;

	DX::ThrowIfFailed(
		device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf())
	);
}




//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void Floor::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	using namespace DirectX;

	auto states = m_commonResources->GetCommonStates();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ワールド行列（床の位置）
	SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::CreateTranslation(m_position);

	// 描画時のエフェクト設定
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_GREATER);
	m_BatchEffect->SetReferenceAlpha(0);
	m_BatchEffect->SetWorld(worldMatrix);
	m_BatchEffect->SetView(view);
	m_BatchEffect->SetProjection(proj);
	m_BatchEffect->SetTexture(m_Texture.Get());
	m_BatchEffect->Apply(context);

	// 描画ステート設定
	
	// 深度ステンシルステート設定
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
	// カリング設定
	context->RSSetState(states->CullCounterClockwise()); 

	// ジオメトリックプリミティブの描画
	m_floorPrimitive->Draw(worldMatrix, view, proj, Colors::White, m_Texture.Get());
}


//---------------------------------------------------------
// 処理
//---------------------------------------------------------
void Floor::Finalize()
{

}
