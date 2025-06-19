//--------------------------------------------------------------------------------------
// File: Star.cpp
//
// 星のパーティクルクラス
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "Star.h"
#include "Libraries/MyLib/BinaryFile.h"
#include "Framework/DeviceResources.h"
#include "StarUtility.h"
#include "Game/UI/UI.h"
#include "Libraries/MyLib/DebugString.h"
#include "Framework/CommonResources.h"

using namespace DirectX;

/// <summary>
/// コンストラクタ
/// </summary>
Star::Star()
	:m_pDR(nullptr)
	,m_timer(0.0f)
	,m_textureIndex(0),
	m_position(SimpleMath::Vector3::Zero),
    m_Billboard{ DirectX::SimpleMath::Matrix::Identity }
	, m_on{false}
{
}

/// <summary>
/// デストラクタ
/// </summary>
Star::~Star()
{
}

/// <summary>
/// テクスチャリソース読み込み関数
/// </summary>
/// <param name="path">相対パス(Resources/Textures/・・・.pngなど）</param>
void Star::LoadTexture(const wchar_t* path)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	DirectX::CreateWICTextureFromFile(m_pDR->GetD3DDevice(), path, nullptr, texture.ReleaseAndGetAddressOf());
	
	m_texture.push_back(texture);
}

/// <summary>
/// 生成関数
/// </summary>
/// <param name="pDR">ユーザーリソース等から持ってくる</param>
void Star::Create(DX::DeviceResources* pDR)
{	
	m_pDR = pDR;
	ID3D11Device1* device = pDR->GetD3DDevice();

	//	シェーダーの作成
	CreateShader();

	//	画像の読み込み
	LoadTexture(L"Resources/Textures/Star.png");

	// プリミティブバッチの作成
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(pDR->GetD3DDeviceContext());

	m_states = std::make_unique<CommonStates>(device);

}
/// <summary>
/// 更新関数
/// </summary>
/// <param name="timer">Game等からStepTimerを受け取る</param>
void Star::Update(float elapsedTime, DirectX::SimpleMath::Vector3 pos)
{
	m_timer += elapsedTime;

	if (m_on)
	{
		CreateCircularParticles(4, 0.5f, 1.0f, 0.1f, 2.0f, pos);
		m_on = false;
	}
	for (auto it = m_starUtility.begin(); it != m_starUtility.end();)
	{
		if (!it->Update(elapsedTime))
		{
			it = m_starUtility.erase(it);
		}
		else
		{
			++it;
		}
	}
	
}

/// <summary>
/// Shader作成部分だけ分離した関数
/// </summary>
void Star::CreateShader()
{
	ID3D11Device1* device = m_pDR->GetD3DDevice();

	//	コンパイルされたシェーダファイルを読み込み
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleStarGS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/StarPS.cso");
	
	//	インプットレイアウトの作成
	device->CreateInputLayout(&UI::INPUT_LAYOUT[0],
		static_cast<UINT>(UI::INPUT_LAYOUT.size()),
		VSData.GetData(), VSData.GetSize(),
		m_inputLayout.GetAddressOf());

	//	頂点シェーダ作成
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	//	ジオメトリシェーダ作成
	if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{// エラー
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}
	//	ピクセルシェーダ作成
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{// エラー
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

/// <summary>
/// 描画関数
/// </summary>
/// <param name="view">ビュー行列</param>
/// <param name="proj">射影行列</param>
void Star::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();

	DirectX::SimpleMath::Vector3 cameraDir = m_cameraTarget - m_cameraPosition;
	cameraDir.Normalize();

	DirectX::SimpleMath::Vector3 containedCameraDir = cameraDir;
	containedCameraDir.y = 0.0f;
	containedCameraDir.Normalize();

	SimpleMath::Vector3 startDir = SimpleMath::Vector3::Forward;
	float dot = containedCameraDir.Dot(startDir);
	float cross = startDir.Cross(containedCameraDir).y;


	// ビルボード行列を作成
	XMVECTOR camPos = XMLoadFloat3(&m_cameraPosition);
	XMVECTOR camTarget = XMLoadFloat3(&m_cameraTarget);
	XMVECTOR look = XMVector3Normalize(camTarget - camPos);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, look));
	up = XMVector3Cross(look, right);

	XMMATRIX billboard = XMMATRIX(
		right,
		up,
		look,
		XMVectorSet(0, 0, 0, 1)
	);

	// ★ この行で m_Billboard にセット
	m_Billboard = billboard;

	//	シェーダーに渡す追加のバッファを作成する。(ConstBuffer）
	ConstBuffer cbuff;
	cbuff.matView = view.Transpose();
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = m_world.Transpose();
	cbuff.Diffuse = SimpleMath::Vector4(dot, cross, 1, static_cast<float>(m_stepTimer.GetTotalSeconds()));

	//	受け渡し用バッファの内容更新(ConstBufferからID3D11Bufferへの変換）
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	シェーダーにバッファを渡す
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	//	画像用サンプラーの登録
	ID3D11SamplerState* sampler[1] = { m_states->PointWrap() };
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

	for (int i = 0; i < m_texture.size(); i++)
	{
		context->PSSetShaderResources(i, 1, m_texture[i].GetAddressOf());
	}
	context->PSSetShaderResources(0, 1, m_texture[m_textureIndex].GetAddressOf());

	//	インプットレイアウトの登録
	context->IASetInputLayout(m_inputLayout.Get());

	//	板ポリゴンを描画
	m_batch->Begin();

	for (StarUtility& li : m_starUtility)
	{
		DirectX::SimpleMath::Vector3 pos = li.GetPosition();

		VertexPositionColorTexture vPCT{};
		vPCT.position = XMFLOAT3(pos);
		vPCT.color = XMFLOAT4(li.GetNowColor());
		vPCT.textureCoordinate = XMFLOAT2(li.GetNowScale().x, 0.0f);

		std::vector<VertexPositionColorTexture> oneVertex = { vPCT };
		m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, oneVertex.data(), 1);
	}
	
	m_batch->End();

	//	シェーダの登録を解除しておく
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

}

/// <summary>
/// パーティクルを円形に生成し、貰った位置を中心に円運動させる
/// </summary>
/// <param name="numParticles">生成するパーティクルの数</param>
/// <param name="radius">円の半径</param>
/// <param name="speed">パーティクルの初速度</param>
void Star::CreateCircularParticles(int numParticles, float maxSpeed, float initialScale, float finalScale, float lifetime,DirectX::SimpleMath::Vector3 pos)
{
	float range = 10.0f;

	std::vector<DirectX::SimpleMath::Vector3> cornerPositions = {
	   {  range, 1.0f, 0.0f },   // 右上
	   {  0.0f,  1.0f, range },  // 左上
	   { -range, 1.0f, 0.0f },   // 右下
	   { 0.0f,   1.0f, -range }  // 左下
	};
	const float TWO_PI = DirectX::XM_2PI;   
	for (int i = 0; i < numParticles; ++i)
	{
		DirectX::SimpleMath::Vector3 initialPosition = pos + cornerPositions[i];

		// 位相オフセットを計算
		float initialAngle = (TWO_PI / numParticles) * i;

		// 速度を中心方向に向ける（外から内に向かう動き）
		DirectX::SimpleMath::Vector3 velocity = pos - initialPosition;
		//velocity.Normalize();
		velocity *= maxSpeed;

		// パーティクルを生成
		StarUtility pU(
			lifetime,  // lifetime
			initialPosition,
			pos,
			velocity,
			DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),  // 加速度なし
			DirectX::SimpleMath::Vector3(initialScale, initialScale, initialScale),
			DirectX::SimpleMath::Vector3(finalScale, finalScale, finalScale),
			DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),  // 白
			DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 0.0f)   
		);
		// ★ 角度を仕込む
		pU.SetInitialAngle(initialAngle);

		m_starUtility.push_back(pU);
	}
}