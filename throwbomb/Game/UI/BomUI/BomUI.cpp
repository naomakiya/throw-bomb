/*
	@file	BomUI.cpp
	@brief	ボムUIクラス
*/
#include "pch.h"
#include "Game/UI/BomUI/BomUI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include <Libraries/MyLib/BinaryFile.h>
#include "Game/UI/UI.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
BomUI::BomUI()
	:
	m_commonResources{},
	m_spriteBatch{},
	m_bomTexture{},
	m_texCenter{},
	m_inputLayout{},
	m_vertexShader{},
	m_pixelShader{},
	m_geometryShader{},
	m_CBuffer{},
	m_time{99},
	m_sin{}

{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
BomUI::~BomUI()
{
	
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void BomUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
{
	assert(resources);
    assert(spriteBatch);
	m_commonResources = resources;
	m_spriteBatch = spriteBatch;

    //	シェーダーの作成
    CreateShader();
    //画像の読み込み
    LoadTexture(L"Resources/Textures/Boms.png");
    m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(m_commonResources->GetDeviceResources()->GetD3DDeviceContext());
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void BomUI::Render(float elapsedTime)
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	//時間経過
	m_time -= elapsedTime;
	
	//	頂点情報(板ポリゴンの４頂点の座標情報）
	DirectX::VertexPositionColorTexture vertex[4] =
	{
		DirectX::VertexPositionColorTexture(DirectX::SimpleMath::Vector3(0.0f,  -0.75f, 0.0f),
		DirectX::SimpleMath::Vector4::One, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
	};

	ConstBuffer cbuff;
	//	シェーダーに渡す追加のバッファを作成する。
	//残り30秒になったら
	if (m_time < 30.0f)
	{
		//大きさの調整
		m_sin = sin((m_time) + 25) / 10 + 0.1f;

		cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld *= DirectX::SimpleMath::Matrix::CreateScale(m_sin + 0.15f, m_sin + 0.275f, 1.0f);
		cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
		cbuff.time = DirectX::SimpleMath::Vector4(m_time, 1, 1, 1);
	}
	else
	{
		
		cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld *= DirectX::SimpleMath::Matrix::CreateScale(0.15f, 0.275f, 1.0f);
		cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
		cbuff.time = DirectX::SimpleMath::Vector4(m_time, 1, 1, 1);
	}
	
	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	半透明描画指定
	ID3D11BlendState* blendstate = states->NonPremultiplied();

	//	透明判定処理
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	深度バッファに書き込み参照する
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	//	カリングは左周り
	context->RSSetState(states->CullNone());

	//	シェーダをセットする
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	context->PSSetShaderResources(0, 1, m_bomTexture.GetAddressOf());
	

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout.Get());

	//	板ポリゴンを描画	//ボムを描画する
	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 1);
	m_batch->End();

	

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
    
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void BomUI::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// Shader作成部分だけ分離
//---------------------------------------------------------
void BomUI::CreateShader()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	//	コンパイルされたシェーダファイルを読み込み
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleGS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticlePS.cso");

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
///	テクスチャリソース読み込み
//---------------------------------------------------------
void BomUI::LoadTexture(const wchar_t* path)
{
    DirectX::CreateWICTextureFromFile(m_commonResources->GetDeviceResources()->GetD3DDevice(), path, nullptr, m_bomTexture.ReleaseAndGetAddressOf());
    
}