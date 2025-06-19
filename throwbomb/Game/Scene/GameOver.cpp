/*
	@file	GameOver.cpp
	@brief	ゲームオーバークラス
*/
#include "pch.h"
#include "GameOver.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include"Framework/Graphics.h"
#include <cassert>
#include "Framework/Graphics.h"
#include <Framework/Resources.h>
#include "Game/Screen.h"
#include <Game/ResourceManager/ResourceManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
GameOver::GameOver(ResourceManager* resourceManager)
	:
	m_commonResources{},
	m_projection{},
	m_isChangeScene{},
	m_position{},
	m_view{},
	m_clear{},
	m_resourceManager{resourceManager}

{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
GameOver::~GameOver()
{
	// do nothing.
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void GameOver::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// スプライトバッチを作成する
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	// 画像をロードする
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/GameOver.png",
			nullptr,
			m_clear.ReleaseAndGetAddressOf()
		)
	);


	// 以下、テクスチャの大きさを求める→テクスチャの中心座標を計算する

	// 一時的な変数の宣言
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
	D3D11_TEXTURE2D_DESC desc;
	Vector2 texSize;

	// テクスチャの情報を取得する================================
	// テクスチャをID3D11Resourceとして見る
	m_clear->GetResource(resource.GetAddressOf());
	// ID3D11ResourceをID3D11Texture2Dとして見る
	resource.As(&tex2D);
	// テクスチャ情報を取得する
	tex2D->GetDesc(&desc);

	// テクスチャサイズを取得し、float型に変換する
	texSize.x = static_cast<float>(desc.Width);
	texSize.y = static_cast<float>(desc.Height);

	// テクスチャの中心位置を計算する
	m_texCenter = texSize / 2.0f;

	// シーン変更フラグを初期化する
	m_isChangeScene = false;
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void GameOver::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	// キーボードステートトラッカーを取得する
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	 //スペースキーが押されたら
	if (kbTracker->pressed.Space)
	{
	    m_isChangeScene = true;
	}


}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void GameOver::Render()
{
	auto states = m_commonResources->GetCommonStates();

	
	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());
	// TRIDENTロゴの描画位置を決める
	DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2::Zero;
	pos = DirectX::SimpleMath::Vector2(360.0f, 180.0f);

	// TRIDENTロゴを描画する
	m_spriteBatch->Draw(m_clear.Get(), pos);

	// デバッグ情報を表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("Result");
	m_spriteBatch->End();
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void GameOver::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID GameOver::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::TITLE;
	  
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}