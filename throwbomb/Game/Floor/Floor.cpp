/*
	@file	Floor.cpp
	@brief	床クラス
*/
#include "pch.h"
#include "Floor.h"
#include <WICTextureLoader.h> 
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
//#include <GeometricPrimitive.h>

//---------------------------------------------------------
// コンストラクト
//---------------------------------------------------------
Floor::Floor(ID3D11Device1* device,  DirectX::SimpleMath::Vector3 position, float width, float depth)
	:m_position(position),
	m_width(width),
	m_depth(depth)
{
	using namespace DirectX;

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
void Floor::Initialize(ID3D11Device1* device, CommonResources* resources)
{
	using namespace DirectX;

	assert(resources);
	// 共通リソースの設定
	m_commonResources = resources;
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	// テクスチャのロード
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Floor.png", // 床のテクスチャファイル
		nullptr,
		m_Texture.GetAddressOf()
	);
	
	// ボックスを作成（サイズ：幅、深さ、非常に薄い高さ）
	XMFLOAT3 size(m_width, 1.0f, m_depth);  // 高さは非常に薄い床として設定
	//立方体の作成
	m_floorPrimitive = GeometricPrimitive::CreateBox(context, size);
	//　バウンディングボックスの作成
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = size;

	// 深度ステンシルステート設定（床用）
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
void Floor::Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	using namespace DirectX;
	auto states = m_commonResources->GetCommonStates();
	// ワールド行列（床の位置、スケーリング）
	SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::CreateTranslation(m_position);

	// 描画時のエフェクト設定
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_GREATER); // 不透明
	m_BatchEffect->SetReferenceAlpha(0);
	m_BatchEffect->SetWorld(worldMatrix);
	m_BatchEffect->SetView(view);
	m_BatchEffect->SetProjection(proj);
	m_BatchEffect->SetTexture(m_Texture.Get());
	m_BatchEffect->Apply(context);

	// 描画ステート設定
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0); // 深度ステンシルステート設定
	context->RSSetState(states->CullCounterClockwise()); // カリング設定

	// ジオメトリックプリミティブの描画
	m_floorPrimitive->Draw(worldMatrix, view, proj, Colors::White, m_Texture.Get());
}


//---------------------------------------------------------
// 処理
//---------------------------------------------------------
void Floor::Finalize()
{

}
