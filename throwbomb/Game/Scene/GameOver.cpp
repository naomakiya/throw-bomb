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
#include <cassert>
#include "Framework/DeviceResources.h"
#include "Game/Screen.h"
#include <Game/ResourceManager/ResourceManager.h>
#include "Game/Sound/Sound.h"
#include "Game/UI/Number/NumberUI.h"
#include "Game/Scene/SceneManager.h"
#include "Game/ResultParamete/ResultParamete.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
GameOver::GameOver(SceneManager* sceneManager)
	:
	m_commonResources{},
	m_projection{},
	m_isChangeScene{},
	m_selectBox{},
	m_scale{},
	m_view{},
	m_gameOver{},
	m_backGround{},
	m_currentSelection{ Selection::ReStart},
	m_timer(0.0f),
	m_sceneManager{sceneManager},
	m_numberUI{},
	m_sumEnemyNumberUI{},
	m_slash{}

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
			ResourceManager::GetTexturePath("GameOver").c_str(),
			nullptr,
			m_gameOver.ReleaseAndGetAddressOf()
		)
	);
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Explosion").c_str(),
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
	// スラッシュ
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Slash").c_str(),
			nullptr,
			m_slash.ReleaseAndGetAddressOf()
		)
	);
	// 残りの敵の数
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("RemainingEnemy").c_str(),
			nullptr,
			m_remainingEnemy.ReleaseAndGetAddressOf()
		)
	);
	int enemy = 0;
	int sumenemy = 0;
	m_sceneManager->GetResultParamete()->SetGameOverScoreResult(enemy,sumenemy);
	m_numberUI = std::make_unique<NumberUI>(static_cast<float>(enemy));
	m_numberUI->Create(m_commonResources->GetDeviceResources(),false);
	m_sumEnemyNumberUI = std::make_unique<NumberUI>(static_cast<float>(sumenemy));
	m_sumEnemyNumberUI->Create(m_commonResources->GetDeviceResources(),false);

	// 一時的な変数の宣言
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
	D3D11_TEXTURE2D_DESC desc;
	Vector2 texSize;

	// テクスチャの情報を取得する================================
	// テクスチャをID3D11Resourceとして見る
	m_gameOver->GetResource(resource.GetAddressOf());
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
	switch (m_currentSelection)
	{
	case Selection::NONE:
		break;
	case Selection::Select:
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
	case Selection::ReStart:
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
	case Selection::Title:
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
		m_isChangeScene = true;
	}
	m_timer += elapsedTime;
	m_scale = 0.125f * sinf(m_timer) + 1.5f;

}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void GameOver::Render()
{
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();
	
	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());
	Vector2 Pos(Screen::CENTER_X, Screen::CENTER_Y);
	// TRIDENTロゴの描画位置を決める
	DirectX::SimpleMath::Vector2 pos = Pos;
	pos.y = Pos .y - 200.0f;
	
	// 
	m_spriteBatch->Draw(m_backGround.Get(),Pos, nullptr, Colors::White , 0.0f,
		Vector2(128.5f, 128.5f), 5.0f);
	m_spriteBatch->Draw(m_gameOver.Get(), pos, nullptr, Colors::White, 0.0f,
		m_texCenter, 2.0f);

	Vector2 slectPos(Screen::CENTER_X - 300, Screen::CENTER_Y + 250);
	Vector2 restartPos(Screen::CENTER_X, Screen::CENTER_Y + 250);
	Vector2 titlePos(Screen::CENTER_X + 300, Screen::CENTER_Y + 250);

	// リトライの描画
	m_spriteBatch->Draw(m_reStart.Get(), restartPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), 1);
	// 選択の描画
	m_spriteBatch->Draw(m_select.Get(), slectPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), 1);

	// タイトルへの描画
	m_spriteBatch->Draw(m_title.Get(), titlePos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), 1);
	// セレクトBOXの描画
	m_spriteBatch->Draw(m_selectBox.Get(), m_slectPos, nullptr, Colors::Black , 0.0f,
		Vector2(100.0f, 50.0f), m_scale);

	//スラッシュ
	m_spriteBatch->Draw(m_slash.Get(),Vector2(Screen::CENTER_X, Screen::CENTER_Y+50), nullptr, Colors::White, 0.0f,
		Vector2(50.0f, 50.0f), 1.5);
	// 残りの敵の数の文字
	m_spriteBatch->Draw(m_remainingEnemy.Get(), Vector2(Screen::CENTER_X, Screen::CENTER_Y - 50), nullptr, Colors::Black, 0.0f,
		Vector2(320.0f, 50.0f), 1);
	
	m_spriteBatch->End();

	m_numberUI->Render(Vector3(-0.3f, -0.2f, 1.0f));
	m_sumEnemyNumberUI->Render(Vector3(0.2f, -0.2f, 1.0f));
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
		switch (m_currentSelection)
		{
		case Selection::NONE:
			break;
		case Selection::ReStart:
			return IScene::SceneID::PLAY;
			break;
		case Selection::Select:
			return IScene::SceneID::SELECT;
			break;
		case Selection::Title:
			return IScene::SceneID::TITLE;
			break;
		default:
			break;
		}

	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}