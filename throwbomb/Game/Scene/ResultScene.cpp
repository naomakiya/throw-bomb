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
#include <cassert>
#include "Framework/DeviceResources.h"
#include "Game/Screen.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "TitleScene.h"
#include "Game/Sound/Sound.h"
#include <Framework/LoadJson.h>
#include "SceneManager.h"
#include "Game/ResultParamete/ResultParamete.h"
#include "Game/UI/Number/NumberUI.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
ResultScene::ResultScene(SceneManager* sceneManager)
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
	m_select{},
	m_sceneManager{sceneManager},
	m_numberUI{},
	m_score{}

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

	// 音量の読み込み
	LoadJson json("Resources/Json/Music.json");
	// BGMの音量の設定
	float m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);

	m_sound = std::make_unique<Sound>();
	m_sound->Initialize();
	// BGMの再生
	m_sound->PlayBGM(ResourceManager::GetBGMPath("ResultBGM").c_str(), true);
	// 音量の設定
	m_sound->SetVolume(m_bgmVolume);


	
	
	m_score = m_sceneManager->GetResultParamete()->GetScore();
	
	m_numberUI = std::make_unique<NumberUI>(static_cast<float>(m_score));
	m_numberUI->Create(m_commonResources->GetDeviceResources(),true);
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
		if (kbTracker->pressed.Left)
		{
			m_currentSelection = Selection::Title;
		}
		else if (kbTracker->pressed.Right)
		{
			m_currentSelection = Selection::ReStart;
		}
		m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X - 300, Screen::CENTER_Y + 250 };
		break;
	case ResultScene::Selection::ReStart:

		if (kbTracker->pressed.Left)
		{
			m_currentSelection = Selection::Select;
		}
		else if (kbTracker->pressed.Right)
		{
			m_currentSelection = Selection::Title;
		}
		m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X , Screen::CENTER_Y + 250 };
		break;
	case ResultScene::Selection::Title:
		if (kbTracker->pressed.Left)
		{
			m_currentSelection = Selection::ReStart;
		}
		else if (kbTracker->pressed.Right)
		{
			m_currentSelection = Selection::Select;
		}

		m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X + 300, Screen::CENTER_Y + 250 };
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

	m_scale = 0.125f * sinf(m_elapsedTime) + 1.5f;
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void ResultScene::Render()
{

	//テクスチャーの描画
	this->TextureRender();
	
	m_numberUI->Render(DirectX::SimpleMath::Vector3(0.0f, -0.2f, 0.0f));
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void ResultScene::Finalize()
{
	// 音の終了処理
	m_sound->Finalize();
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
			ResourceManager::GetTexturePath("Clear").c_str(),
			nullptr,
			m_clear.ReleaseAndGetAddressOf()
		)
	);

	// タイトル背景画像をロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Floor").c_str(),
			nullptr,
			m_backGround.ReleaseAndGetAddressOf()
		)
	);

	//開始テクスチャーをロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("ReTry").c_str(),
			nullptr,
			m_reStart.ReleaseAndGetAddressOf()
		)
	);
	//終わりテクスチャーをロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("TitleButton").c_str(),
			nullptr,
			m_title.ReleaseAndGetAddressOf()
		)
	);
	//セレクトテクスチャーボタンをロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Select").c_str(),
			nullptr,
			m_select.ReleaseAndGetAddressOf()
		)
	);
	// セレクトBox
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Box").c_str(),
			nullptr,
			m_selectBox.ReleaseAndGetAddressOf()
		)
	);
	// スコア
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Score").c_str(),
			nullptr,
			m_scoreTextur.ReleaseAndGetAddressOf()
		)
	);

	// Aスコア
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("A").c_str(),
			nullptr,
			m_aScore.ReleaseAndGetAddressOf()
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
	m_spriteBatch->Draw(m_backGround.Get(), post, nullptr, Colors::White, 0.0f, Vector2(0, 0), 1 * 5.0f);
	Vector2 pos(Screen::CENTER_X, 0.0f);
	m_spriteBatch->Draw(m_clear.Get(), pos, nullptr, {1,1,1,1.0}, 0.0f, Vector2(320, 24), 1 * 1.5);

	
	Vector2 restartPos(Screen::CENTER_X - 300, Screen::CENTER_Y + 250);
	Vector2 slectPos(Screen::CENTER_X, Screen::CENTER_Y + 250);
	Vector2 titlePos(Screen::CENTER_X + 300, Screen::CENTER_Y + 250);

	// 選択肢の描画
	m_spriteBatch->Draw(m_selectBox.Get(), m_slectPos, nullptr, Colors::Black, 0.0f,
		Vector2(100.0f, 50.0f), m_scale);
	// 選択肢の描画
	m_spriteBatch->Draw(m_reStart.Get(), restartPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), 1);
	// 選択肢の描画
	m_spriteBatch->Draw(m_select.Get(), slectPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), 1);

	// エンド選択肢の描画
	m_spriteBatch->Draw(m_title.Get(), titlePos, nullptr, Colors::White , 0.0f,
		Vector2(140, 105), 1);
	// スコアの文字
	m_spriteBatch->Draw(m_scoreTextur.Get(), Vector2(Screen::CENTER_X, Screen::CENTER_Y - 50), nullptr, Colors::Black, 0.0f,
		Vector2(320.0f, 50.0f), 1);
	if (m_score > 20000)
	{
		m_spriteBatch->Draw(m_aScore.Get(), Vector2(Screen::CENTER_X- 500, Screen::CENTER_Y), nullptr, Colors::White, 0.0f,
			Vector2(5.0f, 50.0f), 2.5f);
	}
	m_spriteBatch->End();
}
