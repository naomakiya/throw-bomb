/*
	@file	SelectScene.cpp
	@brief	セレクトシーンクラス
*/
#include "pch.h"
#include "Framework/DeviceResources.h"
#include "SelectScene.h"
#include "Game/Screen.h"
#include "Libraries/MyLib/InputManager.h"
#include"Framework/CommonResources.h"
#include "Game/UI/UI.h"
#include"Game/ResourceManager/ResourceManager.h"
#include <Game/Sound/Sound.h>
#include "SceneManager.h"



//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
SelectScene::SelectScene(SceneManager* sceneManager)
	:
	m_commonResources{},
	m_spriteBatch{ },
	m_fade{  },
	m_one{  },
	m_two{  },
	m_three{},
	m_title{  },
	m_background{  },
	m_texCenter{  },
	m_projection{  },
	m_view{ },
	m_position{},
	m_scale{ 1.0f },
	m_alpha{ 1.0f },
	m_elapsedTime{ 0.0f },
	m_isChangeScene{},
	m_visible{ true }, // 初期状態では表示
	m_IsFade{false},
	m_currentSelection(Selection::NONE),
	m_slectPos{0.0f,0.0f},
	m_ui{},
	m_sceneManager{ sceneManager },
	m_selectBox{},
	m_minScale{},
	m_maxScale{},
	m_selectedScale{0.0f}
{
	

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
SelectScene::~SelectScene()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void SelectScene::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	//CommonResourcesからの使用データを出力
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	
	
	// クラス生成
	ClassCreat();
	//リソースの取り込み
	LoadResource(device);
	
	
	m_minScale = DirectX::SimpleMath::Vector2(1.9875f, 2.25f);
	m_maxScale = m_minScale * 1.1f; // 10%拡大（調整可）

	// 画面のサイズを取得する
	auto outputSize = m_commonResources->GetDeviceResources()->GetOutputSize();
	float screenWidth = static_cast<float>(outputSize.right - outputSize.left);
	float screenHeight = static_cast<float>(outputSize.bottom - outputSize.top);
	
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, DirectX::SimpleMath::Vector3(0, 0, 10), DirectX::SimpleMath::Vector3::UnitY);

	m_projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(45.0f), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

	// シーン変更フラグを初期化
	m_isChangeScene = false;
	m_currentSelection = Selection::ONE;
	Sound::GetInstance().Initialize();
	Sound::GetInstance().PlayBGM(ResourceManager::getBGMPath("TitleBGM").c_str(), true);
	Sound::GetInstance().SetVolume(0.1f);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void SelectScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 経過時間を更新
	m_elapsedTime += elapsedTime;

	UpdateSelection();

	float t = (sinf(m_elapsedTime * 2.0f) + 1.0f) * 0.1f; // 0.0 ～ 1.0 の周期的変化
	m_scale = m_minScale + (m_maxScale - m_minScale) * t;
	
	m_fade->Timer(elapsedTime);

	// 表示状態に基づいてアルファ値を設定
	m_alpha = m_visible ? 1.0f : 0.0f;
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void SelectScene::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//テクスチャーの描画
	TextureRender();
	//フェイド状態か
	if (m_IsFade)
	{
		//フェイドの描画
		m_fade->Render(m_ui.get());

		//1秒以上経ったらシーンを変える
		if (m_fade->GetTime() >= 2.0f)
		{
			m_isChangeScene = true;
		}
		
		return;
	}
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void SelectScene::Finalize()
{
	Sound::GetInstance().Finalize();
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID SelectScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		switch (m_currentSelection)
		{
		case SelectScene::Selection::NONE:
			break;
		case SelectScene::Selection::ONE:
			m_sceneManager->SetSelectNumber(1);
			return IScene::SceneID::PLAY;			
			break;
		case SelectScene::Selection::TWO:
			m_sceneManager->SetSelectNumber(2);
			return IScene::SceneID::PLAY;
			break;
		case SelectScene::Selection::THREE:
			m_sceneManager->SetSelectNumber(3);
			return IScene::SceneID::PLAY;
			break;
		case SelectScene::Selection::TITLE:
			return IScene::SceneID::TITLE;
			break;
		default:
			break;
		}
		
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// クラスを生成する
//---------------------------------------------------------
void SelectScene::ClassCreat()
{
	auto deviceResources = m_commonResources->GetDeviceResources();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ユーザーインターフェイス
	m_ui = std::make_unique<UI>();
	m_ui->Initialize(m_commonResources);
	//フェイドの生成
	m_fade = std::make_unique<Fade>();
	m_fade->Create(deviceResources);
	// スプライトバッチを作成する
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
}

//---------------------------------------------------------
// リソースを取り込む
//---------------------------------------------------------
void SelectScene::LoadResource(ID3D11Device1* device)
{
	// 1をロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("1").c_str(),
			nullptr,
			m_one.ReleaseAndGetAddressOf()
		)
	);
	// 2をロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("2").c_str(),
			nullptr,
			m_two.ReleaseAndGetAddressOf()
		)
	);
	// 3をロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("3").c_str(),
			nullptr,
			m_three.ReleaseAndGetAddressOf()
		)
	);
	// タイトル背景画像をロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("TitleButton").c_str(),
			nullptr,
			m_title.ReleaseAndGetAddressOf()
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
	// タイトル背景画像をロードする
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Box").c_str(),
			nullptr,
			m_selectBox.ReleaseAndGetAddressOf()
		)
	);
}

void SelectScene::TextureRender()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// 背景とタイトルの描画
	Vector2 post(0.0f, 0.0f);
	m_spriteBatch->Draw(m_background.Get(), post, nullptr, Colors::White, 0.0f, Vector2(0, 0), 5.0f);
	Vector2 pos(250.0f, 50.0f);
	//m_spriteBatch->Draw(m_title.Get(), pos, nullptr, Colors::White, 0.0f, Vector2(0, 0), m_scale *1.5);

	Vector2 onePos(Screen::CENTER_X -400, Screen::CENTER_Y-50);
	Vector2 twoPos(Screen::CENTER_X, Screen::CENTER_Y - 50);
	Vector2 threePos(Screen::CENTER_X +400, Screen::CENTER_Y - 50);
	Vector2 endPos(Screen::CENTER_X , Screen::CENTER_Y + 200);

	//1選択肢の描画
	m_spriteBatch->Draw(m_one.Get(), onePos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(639, 358), 0.3f);
	// 2選択肢の描画
	m_spriteBatch->Draw(m_two.Get(), twoPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(637, 356), 0.3f);
	// 3選択肢の描画
	m_spriteBatch->Draw(m_three.Get(), threePos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(637, 356), 0.3f);
	// タイトルの描画
	m_spriteBatch->Draw(m_title.Get(), endPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(140.0f, 105.0f), 1.5f);

	// 選択肢の描画
	m_spriteBatch->Draw(m_selectBox.Get(), m_slectPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(100.0f,50.0f), m_scale);
	m_spriteBatch->End();
}

void SelectScene::UpdateSelection()
{
	// キーボードステートトラッカーを取得する
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();
	if (!m_IsFade)
	{
		switch (m_currentSelection)
		{
		case SelectScene::Selection::NONE:
			break;
		case SelectScene::Selection::ONE:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::THREE;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::TWO;
			}
			else if (kbTracker->pressed.Down)
			{
				m_currentSelection = Selection::TITLE;
			}

			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X - 400, Screen::CENTER_Y - 50 };
			break;
		case SelectScene::Selection::TWO:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::ONE;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::THREE;
			}
			else if (kbTracker->pressed.Down)
			{
				m_currentSelection = Selection::TITLE;
			}
			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X , Screen::CENTER_Y - 50 };
			break;
		case SelectScene::Selection::THREE:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::TWO;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::ONE;
			}
			else if (kbTracker->pressed.Down)
			{
				m_currentSelection = Selection::TITLE;
			}
			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X + 400, Screen::CENTER_Y - 50 };
			break;
		case SelectScene::Selection::TITLE:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::ONE;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::THREE;
			}
			else if (kbTracker->pressed.Up)
			{
				m_currentSelection = Selection::TWO;
			}
			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X , Screen::CENTER_Y +200 };
			break;
		default:
			break;
		}

		// スペースキーが押されたら
		if (kbTracker->pressed.Space && !m_IsFade)
		{
			m_IsFade = true;
			m_fade->SetTime(0.0f);
		}
	}
}


