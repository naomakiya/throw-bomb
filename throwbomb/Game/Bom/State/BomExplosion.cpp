/*
  @file BomExplosion.cpp
  @brief ボムの爆発状態クラス
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomExist.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Wall/Wall.h"
#include "Game/Sound/Sound.h"
#include <Libraries/Microsoft/ReadData.h>
#include "Game/Dithering/Dithering.h"
#include "Game/UI/Smork.h"
#include <Framework/LoadJson.h>
#include <Game/ResourceManager/ResourceManager.h>

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
BomExplosion::BomExplosion(BomState* bomState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
	:
	m_position{},
	m_bomState{ bomState },
	m_currentState{},
	m_explosionSphere{},
	m_commonResources{},
	m_wall{ wall },
    m_scale{0.25f},
	m_boundingSphere{},
	m_cnt{},
	m_player{ player },
	m_elapsedTime(0.0f),
	m_ps{},
	m_constantBuffer{},
	m_smork{},
	m_seVolume{ 0.0f }
{
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = m_scale;
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
BomExplosion::~BomExplosion()
{
	this->Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void BomExplosion::Initialize(CommonResources* resources)
{
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// モデルを読み込む準備
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/exprol");
	// 爆発球の作成
	m_explosionSphere = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("Exprol").c_str(), *fx);

	// 爆発球のエフェクトを設定する
	m_explosionSphere->UpdateEffects([&](DirectX::DX11::IEffect* effect){
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
	//シェーダーの読み取り
	ShaderLoad(device);
	//定数バッファーの作成
	CreateConstanBuffer(device);
	// クラス作成
	ClassCreate();

	// テクスチャー読み込み
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Explosion").c_str(),
			nullptr,
			m_explosionTexture.ReleaseAndGetAddressOf()
		)
	);
	
	// 音量の読み込み
	LoadJson json("Resources/Json/Music.json");
	// BGMの音量の設定
	m_seVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
	m_sound = std::make_unique<Sound>();
	m_sound->Initialize();
	
}

//事前準備
void BomExplosion::PreUpdate()
{
	// 位置情報の更新
	m_position = m_bomState->GetPosition();
	// バウディングスフィアの更新
	m_boundingSphere.Center = m_position;
	// カウント
	m_cnt = 2.0f;
	// 初期スケール
	m_scale = 0.25f;
	// 時間経過
	m_elapsedTime = 0.0f;
	// 煙のセット
	m_smork->SetOn(true);
	// 現在の状態を移動にする
	m_bomState->SetBomPresent(BomState::BomtPresent::EXPLOSION);
	// SEの再生
	m_sound->PlaySE(ResourceManager::GetSEPath("Bom").c_str());
	// 音量の設定
	m_sound->SetVolume(m_seVolume);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void BomExplosion::Update(const float& elapsedTime)
{
	// 時間経過
	m_elapsedTime += elapsedTime;
	//カウントダウン
	m_cnt -= elapsedTime;
	//　壁を破壊するか確認する
	CheckBreak();
	//  パーティクルの更新
	m_smork->Update(elapsedTime, m_position, m_scale);
	//時間経過で膨張速度を制御
	m_scale +=  m_elapsedTime;
	if (m_scale > 2.5){
		m_scale = 2.5f;
	}
	//　大きさを変更する
	m_boundingSphere.Radius = m_scale;
	// 当たり判定の大きさの設定
	m_bomState->SetBoundingSphere(m_boundingSphere);

	// 時間経過後状態を生存状態に変更
	if (m_elapsedTime >= 2.0f){
		m_bomState->ChangeState(m_bomState->GetBomExist());
	}


}

void BomExplosion::PostUpdate()
{
	// 状態を生存にする
	m_bomState->SetBomPresent(Exist);
	// 無い状態にする
	m_bomState->SetExist(false);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void BomExplosion::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();


	// 深度ステンシルステート設定
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
	// 行列計算
	Matrix worldExp = Matrix::CreateScale(Vector3(m_scale * 0.01f)) *
			Matrix::CreateRotationY(m_elapsedTime * 0.5f) *
			Matrix::CreateTranslation(m_position);
	// モデルの描画
	m_dithering->RenderObjects(context, states, projection, view, m_position, worldExp, m_position, m_explosionSphere.get());

	// 座標をまず計算 
	DirectX::SimpleMath::Vector3 headPosition = m_position + DirectX::SimpleMath::Vector3(0, 1.0f, 0);

	// === 3D → 2Dスクリーン座標変換 ===
	DirectX::XMVECTOR screenPosVec = DirectX::XMVector3Project(
		DirectX::XMLoadFloat3(&headPosition),
		0.0f, 0.0f,
		static_cast<float>(m_commonResources->GetDeviceResources()->GetOutputSize().right),
		static_cast<float>(m_commonResources->GetDeviceResources()->GetOutputSize().bottom),
		0.0f, 1.0f,
		projection,
		view,
		DirectX::SimpleMath::Matrix::Identity
	);
	//  パーティクルの描画
	m_smork->Render(view, projection);
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void BomExplosion::Finalize()
{
	// 音の終了処理
	m_sound->Finalize();
}

//---------------------------------------------------------
// クラス作成
//---------------------------------------------------------
void BomExplosion::ClassCreate()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	// ディザリングの生成
	m_dithering = std::make_unique<Dithering>(device);
	// 煙の生成
	m_smork = std::make_unique<Smork>();
	m_smork->Create(m_commonResources->GetDeviceResources());
	// SpriteBatch 初期化
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
}

//---------------------------------------------------------
//壁の破壊をするかしないかの確認
//---------------------------------------------------------
void BomExplosion::CheckBreak()
{

	for (const auto& wall : m_wall){
		// バウンディングスフィアと壁のAABBが衝突しているかをチェック
		bool isHitWall = m_boundingSphere.Intersects(wall->GetBoundingBox());
		if (isHitWall){
			Wall::WallType m_type = wall->GetWallType();
			// ひび割れの状態の壁なら破壊する
			if (m_type == Wall::WallType::Destructible && wall->IsVisible() == false){
				wall->SetVisible(true); // 非表示にする
				wall->SetExist(false);
			}

		}
	}
}

//---------------------------------------------------------
// シェイダーの読み取り
//---------------------------------------------------------
void BomExplosion::ShaderLoad(ID3D11Device* device)
{
	std::vector<uint8_t> psBlob = DX::ReadData(L"Resources/Shaders/ExplosionPS.cso");
	DX::ThrowIfFailed(device->CreatePixelShader(psBlob.data(), psBlob.size(), nullptr, m_ps.ReleaseAndGetAddressOf()));
	if (psBlob.empty()) {
		OutputDebugStringA("Failed to load vertex shader: PointLightPS.cso\n");
	}
}

//---------------------------------------------------------
// 定数バッファを作る
//---------------------------------------------------------
void BomExplosion::CreateConstanBuffer(ID3D11Device* device)
{
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
}
