/*
	@file	ResultScene.cpp
	@brief	リザルトシーンクラス
*/
#include "pch.h"
#include "ResultScene.h"
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
#include "Game/ResourceManager/ResourceManager.h"
#include "TitleScene.h"
#include <Game/Sound/Sound.h>
#include "Game/ResourceManager/ResourceManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
ResultScene::ResultScene()
	:
	m_commonResources{},
	m_projection{},
	m_isChangeScene{},
	m_position{},
	m_view{},
	m_clear{},
	m_currentSelection(Selection::Select),
	m_selectedScale(1.2f),
	m_unselectedScale(1.0f),
	m_replay(false),
	m_isSelect(false),
	m_scale{ 1.0f },
	m_elapsedTime{0},
	m_select{}

{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
ResultScene::~ResultScene()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void ResultScene::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// スプライトバッチを作成する
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	
	LoadResource(device);

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

	// 音のインスタンスの取得　再生
	auto& sound = Sound::GetInstance();
	sound.Initialize();
	sound.PlayBGM(ResourceManager::getBGMPath("ResultBGM").c_str(), true);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void ResultScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// キーボードステートトラッカーを取得する
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// 上下キーで選択を切り替える
	switch (m_currentSelection)
	{
	case ResultScene::Selection::Select:
		if (kbTracker->pressed.Up)
		{
			m_currentSelection = Selection::Title;
		}
		else if (kbTracker->pressed.Down)
		{
			m_currentSelection = Selection::ReStart;
		}
		break;
	case ResultScene::Selection::ReStart:
		
		if (kbTracker->pressed.Up)
		{
			m_currentSelection = Selection::Select;
		}
		else if (kbTracker->pressed.Down)
		{
			m_currentSelection = Selection::Title;
		}
		break;
	case ResultScene::Selection::Title:
		if (kbTracker->pressed.Up)
		{
			m_currentSelection = Selection::ReStart;
		}
		else if (kbTracker->pressed.Down)
		{
			m_currentSelection = Selection::Select;
		}
		break;
	default:
		break;
	}
	

	// スペースキーが押されたら
	if (kbTracker->pressed.Space)
	{
		switch (m_currentSelection)
		{
		case ResultScene::Selection::Select:
			m_isSelect = true;
			break;
		case ResultScene::Selection::ReStart:
			m_replay = true;
			break;
		case ResultScene::Selection::Title:
			m_isChangeScene = true;;
			break;
		default:
			break;
		}
	}

	// 経過時間を更新
	m_elapsedTime += elapsedTime;

	m_selectedScale = (sin(m_elapsedTime) + 2) / 3;
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void ResultScene::Render()
{

	//テクスチャーの描画
	this->TextureRender();
	
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void ResultScene::Finalize()
{
	Sound::GetInstance().Finalize();
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID ResultScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::TITLE;
	  
	}
	if (m_replay)
	{
		return IScene::SceneID::PLAY;
	}
	if (m_isSelect)
	{
		return IScene::SceneID::SELECT;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

void ResultScene::LoadResource(ID3D11Device1* device)
{
	// 画像をロードする
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Clear").c_str(),
			nullptr,
			m_clear.ReleaseAndGetAddressOf()
		)
	);

	// タイトル背景画像をロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Floor").c_str(),
			nullptr,
			m_background.ReleaseAndGetAddressOf()
		)
	);

	//開始テクスチャーをロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("ReTry").c_str(),
			nullptr,
			m_reStart.ReleaseAndGetAddressOf()
		)
	);
	//終わりテクスチャーをロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("TitleButton").c_str(),
			nullptr,
			m_title.ReleaseAndGetAddressOf()
		)
	);
	//セレクトテクスチャーボタンをロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Select").c_str(),
			nullptr,
			m_select.ReleaseAndGetAddressOf()
		)
	);
}

void ResultScene::TextureRender()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// 背景とステージクリアの描画（変更なし）
	Vector2 post(0.0f, 0.0f);
	m_spriteBatch->Draw(m_background.Get(), post, nullptr, Colors::White, 0.0f, Vector2(0, 0), m_scale * 5.0f);
	Vector2 pos(Screen::CENTER_X, 50.0f);
	m_spriteBatch->Draw(m_clear.Get(), pos, nullptr, {1,1,1,1.0}, 0.0f, Vector2(320, 24), m_scale * 1.5);

	// スタートとエンド選択肢の描画
	float restartScale = (m_currentSelection == Selection::ReStart) ? m_selectedScale : m_unselectedScale;
	float selectScale = (m_currentSelection == Selection::Select) ? m_selectedScale : m_unselectedScale;
	float titleScale = (m_currentSelection == Selection::Title) ? m_selectedScale : m_unselectedScale;

	Vector2 slectPos(Screen::CENTER_X, Screen::CENTER_Y );
	Vector2 restartPos(Screen::CENTER_X, Screen::CENTER_Y + 125);
	Vector2 titlePos(Screen::CENTER_X, Screen::CENTER_Y + 250);

	// 選択肢の描画
	m_spriteBatch->Draw(m_reStart.Get(), restartPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), restartScale);
	// 選択肢の描画
	m_spriteBatch->Draw(m_select.Get(), slectPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), selectScale);

	// エンド選択肢の描画
	m_spriteBatch->Draw(m_title.Get(), titlePos, nullptr, Colors::White , 0.0f,
		Vector2(140, 105), titleScale);

	m_spriteBatch->End();
}
