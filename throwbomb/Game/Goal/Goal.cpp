/*
  @file  Goal.cpp
  @brief ゴールクラス
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Goal/Goal.h"
#include <Libraries/Microsoft/ReadData.h>

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Goal::Goal(DirectX::SimpleMath::Vector3 pos)
	:
	m_commonResources{},
	m_model{},
	m_boundingSphere{},
	m_position{ pos },
	m_scale{0.025f},
	m_time{0.0f}
{
	//バウディングスフィアの設定
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = 1.0f;
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Goal::~Goal()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void Goal::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	
	//モデルの読み取り
	LoadModel(device);
	//シェーダーの読み取り
	LoadShader(device);
	//定数バッファーの作成
	CreateConstanBuffer(device);
}


//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void Goal::Render(ID3D11DeviceContext* context,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();

	// ワールド行列を更新する
	Matrix world = Matrix::CreateScale(m_scale);
	world *= Matrix::CreateTranslation(m_position);

	//マップリソースの作成
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// 定数バッファをマップする
	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//定数バッファにデータを設定する
	PointLight* cb = static_cast<PointLight*>(mappedResource.pData);
	cb->position = Vector4{ m_position.x,m_position.y + 2.0f,m_position.z + 1.0f, 0.0f };
	cb->color = Vector4{ 2.0f,2.0f,0.0f,0.0f }; // 1より大きくしたことにより光を強くする(黄色）
	cb->time = Vector4{ m_time,0.0f,0.0f,0.0f };

	// マップを解除する
	context->Unmap(m_constantBuffer.Get(), 0);

	//描画
	m_model->Draw(context, *states, world, view, projection, false, [&]()
		{
			// 定数バッファを指定する
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, cbuf);

			// シェーダを設定する
			context->PSSetShader(m_ps.Get(), nullptr, 0);

			// サンプラーステートを指定する
			ID3D11SamplerState* sampler[] = { states->LinearWrap() };
			context->PSSetSamplers(0, 1, sampler);
		});

	// シェーダーを解除
	context->PSSetShader(nullptr, nullptr, 0);

}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void Goal::Finalize()
{
}

//---------------------------------------------------------
// モデルを読み取る
//---------------------------------------------------------
void Goal::LoadModel(ID3D11Device* device)
{
	//// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//モデルをロードする
	m_model = DirectX::Model::CreateFromCMO(device, L"Resources/Models/TreasureChest.cmo", *fx);

	// トーラスモデルのエフェクトを設定する
	m_model->UpdateEffects([&](DirectX::DX11::IEffect* effect)
		{
			auto basicEffect = dynamic_cast<DirectX::DX11::BasicEffect*>(effect);
			if (basicEffect){
				basicEffect->SetLightingEnabled(true);
				basicEffect->SetPerPixelLighting(true);
				basicEffect->SetTextureEnabled(true);
				basicEffect->SetVertexColorEnabled(false);
				basicEffect->SetFogEnabled(false);
			}
		}
	);
}

//---------------------------------------------------------
// シェイダーの読み取り
//---------------------------------------------------------
void Goal::LoadShader(ID3D11Device* device)
{
	std::vector<uint8_t> psBlob = DX::ReadData(L"Resources/Shaders/PointLightPS.cso");
	DX::ThrowIfFailed(device->CreatePixelShader(psBlob.data(), psBlob.size(), nullptr, m_ps.ReleaseAndGetAddressOf()));
	if (psBlob.empty()) {
		OutputDebugStringA("Failed to load vertex shader: PointLightPS.cso\n");
	}
}

//---------------------------------------------------------
// 定数バッファを作る
//---------------------------------------------------------
void Goal::CreateConstanBuffer(ID3D11Device* device)
{
	// 定数バッファ用のバッファオブジェクトを作成する
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(PointLight));	
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf())
	);
}

