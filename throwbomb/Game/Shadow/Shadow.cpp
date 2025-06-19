/*
	@file	Shadow.cpp
	@brief	丸影を表示するクラス
*/
#include "pch.h"
#include "Shadow.h"

using namespace DirectX;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Shadow::Shadow()
	:
	m_texture{},
	m_primitiveBatch{},
	m_basicEffect{},
	m_inputLayout{},
	m_depthStencilState{},
	m_alphaBlendState{}
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void Shadow::Initialize(
	ID3D11Device* device,
	ID3D11DeviceContext* context
)
{
	assert(device);
	assert(context);

	// ベーシックエフェクトを設定する
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(false);
	m_basicEffect->SetTextureEnabled(true);
	m_basicEffect->SetLightingEnabled(false);

	// 入力レイアウトを設定する
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	// プリミティブバッチを生成する
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// 画像を読み込む
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/Shadow.png",
			nullptr,
			m_texture.ReleaseAndGetAddressOf()
		)
	);

	// 深度ステンシルバッファを設定する
	D3D11_DEPTH_STENCIL_DESC desc{};
	desc.DepthEnable = true;                                    // 深度テストを行う
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			// 深度バッファを更新する
	desc.DepthFunc = D3D11_COMPARISON_LESS;                     // 深度値より小さい場合更新

	desc.StencilEnable = false;                                 // ステンシルテストを行う
	desc.StencilReadMask = D3D11_DEPTH_WRITE_MASK_ZERO;			// 0xff
	desc.StencilWriteMask = D3D11_COMPARISON_LESS_EQUAL;		// 0xff

	// 表面
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;        // 参照値とステンシル値が同値なら
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;       // OK 何もしない
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;       // NG 何もしない
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // NG 何もしない

	// 裏面も同じ設定
	desc.BackFace = desc.FrontFace;

	// 深度ステンシルステートを作成する
	DX::ThrowIfFailed(device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf()));

	// アルファブレンドステートを作成する部分をここに追加
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DX::ThrowIfFailed(device->CreateBlendState(&blendDesc, m_alphaBlendState.ReleaseAndGetAddressOf()));
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void Shadow::Render(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection,
	const DirectX::SimpleMath::Vector3& position,
	float radius
)
{
	using namespace DirectX::SimpleMath;

	assert(context);
	assert(states);

	// 深度ステンシルステートを適用する
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);	// 参照値：1

	// カリングを設定する
	context->RSSetState(states->CullCounterClockwise());

	// テクスチャサンプラを適用する
	ID3D11SamplerState* sampler = states->LinearClamp();
	context->PSSetSamplers(0, 1, &sampler);

	// 入力レイアウトを適用する
	context->IASetInputLayout(m_inputLayout.Get());

	// エフェクトを設定し適用する
	m_basicEffect->SetWorld(Matrix::Identity);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(projection);
	m_basicEffect->SetTexture(m_texture.Get());
	m_basicEffect->Apply(context);

	// 影ポリゴンの頂点情報を定義する
	VertexPositionTexture vertices[] = {
		VertexPositionTexture(Vector3::Zero, Vector2(0.0f, 0.0f)),
		VertexPositionTexture(Vector3::Zero, Vector2(1.0f, 0.0f)),
		VertexPositionTexture(Vector3::Zero, Vector2(0.0f, 1.0f)),
		VertexPositionTexture(Vector3::Zero, Vector2(1.0f, 1.0f)),
	};

	// 影ポリゴンのインデックス情報を定義する：DrawIndexedで描画するときに必要
	uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

	// 影ポリゴンの頂点情報を設定する（Y座標を少し浮かせる）
	float shadowOffset = 0.1f;  // 少し上げる
	float scaleFactor = 1.0f;   // 影の大きさを2倍にするスケール係数
	// 影ポリゴンの位置を設定
	vertices[0].position = Vector3(-radius * scaleFactor, shadowOffset, -radius * scaleFactor) + position;
	vertices[1].position = Vector3(radius * scaleFactor, shadowOffset, -radius * scaleFactor) + position;
	vertices[2].position = Vector3(-radius * scaleFactor, shadowOffset, radius * scaleFactor) + position;
	vertices[3].position = Vector3(radius * scaleFactor, shadowOffset, radius * scaleFactor) + position;

	// アルファブレンドステートを適用する部分
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(m_alphaBlendState.Get(), blendFactor, 0xffffffff);

	// 影ポリゴンを描画する
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		indexes,
		_countof(indexes),
		vertices,
		_countof(vertices)
	);
	m_primitiveBatch->End();

	// ブレンドステートをリセット
	context->OMSetBlendState(nullptr, nullptr, 0xffffffff);


	// ブレンドステートをリセット
	context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}
