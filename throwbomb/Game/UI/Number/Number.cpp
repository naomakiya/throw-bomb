/*
	@file	Number.cpp
	@brief	数字のクラス
*/
#include "pch.h"
#include "Number.h"
#include "Libraries/MyLib/BinaryFile.h"
#include "Framework/DeviceResources.h"
#include "Game/UI/UI.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Number::Number()
	: m_pDR(nullptr)
	, m_time{ 1000.0f } // 100秒　１の位が少数第一
	
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Number::~Number()
{
}

//---------------------------------------------------------
// テクスチャリソース読み込み
//---------------------------------------------------------
void Number::LoadTexture(const wchar_t* path)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	DirectX::CreateWICTextureFromFile(m_pDR->GetD3DDevice(), path, nullptr, texture.ReleaseAndGetAddressOf());

	m_texture.push_back(texture);
}

//---------------------------------------------------------
// 生成
//---------------------------------------------------------
void Number::Create(DX::DeviceResources* pDR)
{
	m_pDR = pDR;
	ID3D11Device1* device = pDR->GetD3DDevice();

	//	シェーダーの作成
	CreateShader();

	//ロードするテクスチャー
	LoadTexture(L"Resources/Textures/number.png");

	//	プリミティブバッチの作成
	m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(pDR->GetD3DDeviceContext());

	m_states = std::make_unique<DirectX::CommonStates>(device);

}

//---------------------------------------------------------
// Shader作成部分だけ分離
//---------------------------------------------------------
void Number::CreateShader()
{
	ID3D11Device1* device = m_pDR->GetD3DDevice();

	//	コンパイルされたシェーダファイルを読み込み
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleGS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/NUmberPS.cso");

	//	インプットレイアウトの作成
	device->CreateInputLayout(&UI::INPUT_LAYOUT[0],
		static_cast<UINT>(UI::INPUT_LAYOUT.size()),
		VSData.GetData(), VSData.GetSize(),
		m_inputLayout.GetAddressOf());

	//	頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{//	エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	//	ジオメトリシェーダ作成
	if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{//	エラー
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}
	//	ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{//	エラー
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	//	シェーダーにデータを渡すためのコンスタントバッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void Number::Render(float elapsedTime)
{
	// カウントダウン
	m_time -= elapsedTime * 10.0f; //１０倍してカウントダウンをする

	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();

	//	頂点情報(板ポリゴンの４頂点の座標情報）
	DirectX::VertexPositionColorTexture vertex[4] =
	{ 
		DirectX::VertexPositionColorTexture(DirectX::SimpleMath::Vector3(-0.05f,  -0.85f, 0.0f),
		DirectX::SimpleMath::Vector4::One, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
	};

	//	シェーダーに渡す追加のバッファを作成する。(ConstBuffer）
	ConstBuffer cbuff;
	cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matWorld *= DirectX::SimpleMath::Matrix::CreateScale(0.25f, 0.25f, 1.0f);
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.time = DirectX::SimpleMath::Vector4(m_time, 1, 1, 1);


	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.time = DirectX::SimpleMath::Vector4(m_time, 1, 1, 1);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { m_states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = m_states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	//	カリングは左周り
	context->RSSetState(m_states->CullNone());

	//	シェーダをセットする
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	//	ピクセルシェーダにテクスチャを登録する。
	for (int i = 0; i < m_texture.size(); i++)
	{
		context->PSSetShaderResources(i, 1, m_texture[i].GetAddressOf());
	}

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout.Get());

	//	板ポリゴンを描画
	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 1);
	m_batch->End();

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

}
