/*
	@file	Scene.cpp
	@brief	一般的なシーンクラス
*/
#include "pch.h"
#include "Game/UI/Bom/ButtonUI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include <cassert>
#include <Game/Screen.h>
#include <SpriteBatch.h>
#include <Libraries/MyLib/BinaryFile.h>
#include"Libraries/MyLib/InputManager.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

///	<summary>
///	インプットレイアウト
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> ButtonUI::INPUT_LAYOUT =
{
    { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
ButtonUI::ButtonUI()
	:
	m_commonResources{},
	m_spriteBatch{},
	m_texture{},
	m_texCenter{},
	m_inputLayout{},
	m_vertexShader{},
	m_pixelShader{},
	m_geometryShader{},
	m_CBuffer{},
	m_time{},
	m_sin{},
	m_overlayAlpha{}

{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
ButtonUI::~ButtonUI()
{
	
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void ButtonUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
{
	assert(resources);
    assert(spriteBatch);
	m_commonResources = resources;
	m_spriteBatch = spriteBatch;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

   

    //	シェーダーの作成
    CreateShader();
    //画像の読み込み
    LoadTexture(L"Resources/Textures/A.png");
    m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(m_commonResources->GetDeviceResources()->GetD3DDeviceContext());

}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void ButtonUI::Update()
{
	
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void ButtonUI::Render()
{
	// キーボードの状態を取得する
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();
	m_time -= 0.016f;
	
	//	頂点情報(板ポリゴンの４頂点の座標情報）
	DirectX::VertexPositionColorTexture vertex[4] =
	{
		DirectX::VertexPositionColorTexture(DirectX::SimpleMath::Vector3(-0.75f,  -0.65f, 0.0f),
		DirectX::SimpleMath::Vector4::One, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
	};


	m_overlayAlpha = kbState.IsKeyDown(Keyboard::Keys::Up) ? 0.5f : 0.0f;

	ConstBuffer cbuff;
	cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
	cbuff.matWorld *= DirectX::SimpleMath::Matrix::CreateScale(m_sin + 0.15f, m_sin + 0.275f, 1.0f);
	cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
	cbuff.time = DirectX::SimpleMath::Vector4(m_overlayAlpha, 1, 1, 1);

	
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

	//	ピクセルシェーダにテクスチャを登録する。
	for (int i = 0; i < m_numberTexture.size(); i++)
	{
		context->PSSetShaderResources(i, 1, m_numberTexture[i].GetAddressOf());
	}

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
void ButtonUI::Finalize()
{
	// do nothing.
}

///	<summary>
///	Shader作成部分だけ分離した関数
///	</summary>
void ButtonUI::CreateShader()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	//	コンパイルされたシェーダファイルを読み込み
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/UI/ParticleVS.cso");
	BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/UI/ParticleGS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/UI/ButnnPS.cso");

	//	インプットレイアウトの作成
	device->CreateInputLayout(&INPUT_LAYOUT[0],
		static_cast<UINT>(INPUT_LAYOUT.size()),
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

///	<summary>
///	テクスチャリソース読み込み関数
///	</summary>
///	<param name="path">相対パス(Resources/Textures/・・・.pngなど）</param>
void ButtonUI::LoadTexture(const wchar_t* path)
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
    DirectX::CreateWICTextureFromFile(m_commonResources->GetDeviceResources()->GetD3DDevice(), path, nullptr, texture.ReleaseAndGetAddressOf());

    m_numberTexture.push_back(texture);
}