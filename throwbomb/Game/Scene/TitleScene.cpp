/*
	@file	TitleScene.cpp
	@brief  タイトルシーンクラス
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "TitleScene.h"
#include "Game/Screen.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/UI/UI.h"
#include <Game/Sound/Sound.h>
#include "Libraries/MyLib/Utils.h"
#include <Framework/LoadJson.h>
#include "Game/TitleOp/TitleOp.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
TitleScene::TitleScene()
	:
	m_commonResources{nullptr},
	m_spriteBatch{ },
	m_fade{  },
	m_space{  },
	m_end{  },
	m_title{  },
	m_background{  },
	m_projection{  },
	m_view{ },
	m_position{},
	m_scale{ 1.0f },
	m_alpha{ 1.0f },
	m_elapsedTime{ 0.0f },
	m_isChangeScene{ false },
	m_visible{ true }, 
	m_IsFade{false},
	m_currentSelection(Selection::Start),
	m_currentOptionSelection(OptionSelection::BGM),
	m_selectedScale(1.2f),
	m_unselectedScale(1.0f),
	m_ui{},
	m_bgmVolume{0.0f},
	m_optionSelection{0},
	m_seVolume{0.0f},
	m_TOp{ nullptr },
	m_hasInput{ false }
{
	

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
TitleScene::~TitleScene()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void TitleScene::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//CommonResourcesからの使用データを出力
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	//クラス作成
	CreateClass();

	//リソースの取り込み
	LoadResource(device);

	// 画面のサイズを取得する
	auto outputSize = m_commonResources->GetDeviceResources()->GetOutputSize();
	float screenWidth = static_cast<float>(outputSize.right - outputSize.left);
	float screenHeight = static_cast<float>(outputSize.bottom - outputSize.top);


	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, DirectX::SimpleMath::Vector3(0, 0, 10), DirectX::SimpleMath::Vector3::UnitY);

	m_projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(45.0f), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 500.0f);

	LoadJson json("Resources/Json/Music.json");

	m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
	m_seVolume = json.GetJson()["SE"].value("Volume", 0.0f);

	// 音のインスタンスの取得　再生
	auto& sound = Sound::GetInstance();
	sound.Initialize();
	sound.PlayBGM(ResourceManager::getBGMPath("TitleBGM").c_str(), true);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void TitleScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// 経過時間を更新
	m_elapsedTime += elapsedTime;

	m_TOp->Update(elapsedTime);

	//キーボード操作関係
	if(!m_IsFade) KeyboardOperation();

	//選んでいるシーンの大きさの変更
	m_selectedScale = (sin(m_elapsedTime) + 5.0f) / 5.0f;

	//1秒以上経ったらシーンを変える
	if (m_fade->GetTime() >= 1.0f) {
		m_isChangeScene = true;
	}

	// 表示状態に基づいてアルファ値を設定
	m_alpha = m_visible ? 1.0f : 0.0f;

	//フェイド状態ならフェイドの時間を進める
	if (m_IsFade) {
		m_fade->Timer(elapsedTime);
	}
	
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void TitleScene::Render()
{
	//テクスチャーの描画
	TextureRender();

	//オプションテクスチャー表示
	if(m_isOptionActive) RenderOption();

	if(!m_hasInput) m_TOp->Render(m_view, m_projection);

	//フェイド状態か
	if (m_IsFade){
		m_fade->Render(nullptr);
	}
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void TitleScene::Finalize()
{
	Sound::GetInstance().Finalize();
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID TitleScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::SELECT;
	}

	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// クラスを生成
//---------------------------------------------------------
void TitleScene::CreateClass()
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto deviceResources = m_commonResources->GetDeviceResources();

	//フェイドの生成
	m_fade = std::make_unique<Fade>();
	m_fade->Create(deviceResources);

	// ユーザーインターフェイスの衛星
	m_ui = std::make_unique<UI>();
	m_ui->Initialize(m_commonResources);

	// スプライトバッチを作成する
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	m_TOp = std::make_unique<TitleOp>();
	m_TOp->Initialize(m_commonResources);
}

//---------------------------------------------------------
// リソースを取り込む
//---------------------------------------------------------
void TitleScene::LoadResource(ID3D11Device1* device)
{
	LoadTexture(device, "TitleName", m_title);
	LoadTexture(device, "Strat", m_space);
	LoadTexture(device, "Finish", m_end);
	LoadTexture(device, "Floor", m_background);
	LoadTexture(device, "Option", m_option);
	LoadTexture(device, "Frame", m_optionFreame);
	LoadTexture(device, "BGM", m_bgm);
	LoadTexture(device, "SE", m_se);
	LoadTexture(device, "Box", m_box);
	LoadTexture(device, "VolumeBox", m_volumeBox);
	LoadTexture(device, "Close", m_close);
	LoadTexture(device, "Boms", m_bom);
}

//---------------------------------------------------------
// リソースの読み込み
//---------------------------------------------------------
void TitleScene::LoadTexture(ID3D11Device1* device, const std::string& name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture)
{
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath(name).c_str(),
			nullptr,
			texture.ReleaseAndGetAddressOf()
		)
	);
}

//---------------------------------------------------------
// キーボード操作
//---------------------------------------------------------
void TitleScene::KeyboardOperation()
{
	// キーボードステートトラッカーを取得する
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();
	
	//設定画面を開いている時
	if (m_isOptionActive)
	{
		//設定画面のキーボード操作
		HandleOptionInput(kbTracker.get());
	}
	else
	{
		//タイトル操作
		HandleTitleMenuInput(kbTracker.get());
	}

	if (kbTracker->pressed.Up || kbTracker->pressed.Down || kbTracker->pressed.Space || kbTracker->pressed.Left || kbTracker->pressed.Right)
	{
		m_hasInput = true;
	}
	else
	{
		if (20.0f < m_elapsedTime && !m_isOptionActive)
		{
			m_hasInput = false;
			// タイトル画面の状態をリセットする
			m_TOp->ResetBombs();
			// タイマーをゼロにする
			m_elapsedTime = 0.0f;
		}
	}
}

//---------------------------------------------------------
// 画像の描画
//---------------------------------------------------------
void TitleScene::TextureRender()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();

	// スプライトの描画開始
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// 背景とタイトルの描画（変更なし）
	m_spriteBatch->Draw(m_background.Get(), Vector2(0, 0), nullptr, Colors::White, 0.0f,
		Vector2(0, 0), m_scale * 5.0f);

	// タイトル画像の描画
	m_spriteBatch->Draw(m_title.Get(), Vector2(200.0f, 50.0f), nullptr, Colors::White, 0.0f,
		Vector2(0, 0), m_scale * 1.5f);

	// スタートとエンド選択肢の描画
	float startScale = (m_currentSelection == Selection::Start) ? m_selectedScale : m_unselectedScale;
	float optionScale = (m_currentSelection == Selection::Option) ? m_selectedScale : m_unselectedScale;
	float endScale = (m_currentSelection == Selection::End) ? m_selectedScale : m_unselectedScale;

	// スタートの位置
	Vector2 startPos(Screen::CENTER_X , Screen::CENTER_Y - 50);
	// オプションの位置
	Vector2 optionPos(Screen::CENTER_X, Screen::CENTER_Y + 100);
	// エンドの位置
	Vector2 endPos(Screen::CENTER_X, Screen::CENTER_Y + 275);

	// スタート選択肢の描画
	m_spriteBatch->Draw(m_space.Get(), startPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(300, 50), startScale);
	// オプション選択肢の描画
	m_spriteBatch->Draw(m_option.Get(), optionPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(200, 50), optionScale);
	// エンド選択肢の描画
	m_spriteBatch->Draw(m_end.Get(), endPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(200, 100), endScale);

	// スプライトの描画終了
	m_spriteBatch->End();
}

//---------------------------------------------------------
// オプションの表示
//---------------------------------------------------------
void TitleScene::RenderOption()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();

	// 選択インジケータ（m_bom）の位置を決めて描画
	Vector2 selectorOffset = Vector2(50, 80); // 左にずらす & 中心よりちょい下

	// スプライトの描画開始
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	//（オプションフレーム
	Vector2 framePos(Screen::CENTER_X - 300, 50);
	m_spriteBatch->Draw(m_optionFreame.Get(), framePos, nullptr, Colors::White, 0.0f,Vector2(0, 0), m_scale * 2.5f);

	//=== 各項目の描画位置のベース ===
	const float itemSpacing = 180.0f;
	Vector2 itemPos = framePos + Vector2(0, 80); // フレームの中にオフセット

	//=== 選択色の定義 ===
	auto normalColor = Colors::White;
	auto selectedColor = Colors::Yellow;

	//=== BGM項目 ===
	auto bgmColor = (m_currentOptionSelection == OptionSelection::BGM) ? selectedColor : normalColor;
	m_spriteBatch->Draw(m_bgm.Get(), itemPos, nullptr, bgmColor, 0.0f, Vector2(0, 0), 1.0f);

	// BGMバーの描画
	Vector2 bgmBarPos = itemPos + Vector2(300.0f, 30.0f);
	m_spriteBatch->Draw(m_box.Get(), bgmBarPos, nullptr, Colors::Gray, 0.0f, Vector2(0, 0), 1.0f);

	// ボリュームボックスのソース矩形（横幅で音量を表す）
	RECT bgmSrc = { 0, 0, static_cast<LONG>(200 * m_bgmVolume), 100 };
	m_spriteBatch->Draw(m_volumeBox.Get(), bgmBarPos, &bgmSrc, Colors::Green, 0.0f, Vector2(0, 0), 1.0f);

	// BGMが選択されている場合
	if (m_currentOptionSelection == OptionSelection::BGM) {
		m_spriteBatch->Draw(
			m_bom.Get(),
			itemPos + selectorOffset,
			nullptr,
			Colors::White,
			0.0f,
			Vector2(90, 90),
			m_selectedScale *0.5f
		);
	}
	//=== SE項目 ===
	itemPos.y += itemSpacing;
	auto seColor = (m_currentOptionSelection == OptionSelection::SE) ? selectedColor : normalColor;
	m_spriteBatch->Draw(m_se.Get(), itemPos, nullptr, seColor, 0.0f, Vector2(0, 0), 1.0f);

	// SEバーの描画
	Vector2 seBarPos = itemPos + Vector2(300, 30 );
	m_spriteBatch->Draw(m_box.Get(), seBarPos, nullptr, Colors::Gray, 0.0f, Vector2(0, 0), 1.0f);

	RECT seSrc = { 0, 0, static_cast<LONG>(200 * m_seVolume), 100 };
	m_spriteBatch->Draw(m_volumeBox.Get(), seBarPos, &seSrc, Colors::Green, 0.0f, Vector2(0, 0), 1.0f);
	// SEが選択されている場合
	if (m_currentOptionSelection == OptionSelection::SE) {
		m_spriteBatch->Draw(
			m_bom.Get(),
			itemPos  + selectorOffset,
			nullptr,
			Colors::White,
			0.0f,
			Vector2(90, 90),
			m_selectedScale * 0.5f
		);
	}

	//=== 閉じる項目 ===
	itemPos.y += itemSpacing;
	auto closeColor = (m_currentOptionSelection == OptionSelection::END) ? selectedColor : normalColor;
	m_spriteBatch->Draw(m_close.Get(), itemPos + Vector2(50, 0), nullptr, closeColor, 0.0f, Vector2(0, 0), 1.0f);

	// ENDが選択されている場合
	if (m_currentOptionSelection == OptionSelection::END) {
		m_spriteBatch->Draw(
			m_bom.Get(),
			itemPos + selectorOffset,
			nullptr,
			Colors::White,
			0.0f,
			Vector2(90, 90),
			m_selectedScale * 0.5f
		);
	}
	// スプライトの描画終了
	m_spriteBatch->End();
	
}

//---------------------------------------------------------
// オプション操作
//---------------------------------------------------------
void TitleScene::HandleOptionInput(const DirectX::Keyboard::KeyboardStateTracker* tracker)
{
	if (tracker->pressed.Up) {
		if (m_currentOptionSelection == OptionSelection::SE)
			m_currentOptionSelection = OptionSelection::BGM;
		else if (m_currentOptionSelection == OptionSelection::END)
			m_currentOptionSelection = OptionSelection::SE;
	}

	if (tracker->pressed.Down) {
		if (m_currentOptionSelection == OptionSelection::BGM)
			m_currentOptionSelection = OptionSelection::SE;
		else if (m_currentOptionSelection == OptionSelection::SE)
			m_currentOptionSelection = OptionSelection::END;
	}

	if (tracker->pressed.Left) {
		AdjustVolume(-0.1f);
	}

	if (tracker->pressed.Right) {
		AdjustVolume(0.1f);
	}

	// 音量適用
	Sound::GetInstance().SetVolume(m_bgmVolume);
	// オプション終了
	if (tracker->pressed.Space && m_currentOptionSelection == OptionSelection::END) {
		m_isOptionActive = false;
		m_elapsedTime = 0.0f;
		m_currentSelection = Selection::Option;
	}
}

//---------------------------------------------------------
// タイトル操作
//---------------------------------------------------------
void TitleScene::HandleTitleMenuInput(const DirectX::Keyboard::KeyboardStateTracker* tracker)
{
	if (tracker->pressed.Up) {
		m_currentSelection = static_cast<Selection>((static_cast<int>(m_currentSelection) - 1 + 3) % 3);
	}
	else if (tracker->pressed.Down) {
		m_currentSelection = static_cast<Selection>((static_cast<int>(m_currentSelection) + 1) % 3);
	}

	if (tracker->pressed.Space && m_hasInput) {
		switch (m_currentSelection) {
		case Selection::Start:
			m_IsFade = true;
			break;
		case Selection::Option:
			m_isOptionActive = true;
			break;
		case Selection::End:
			PostQuitMessage(0);
			break;
		}
	}
}

//---------------------------------------------------------
// ボリューム調整
//---------------------------------------------------------
void TitleScene::AdjustVolume(float delta)
{
	if (m_currentOptionSelection == OptionSelection::BGM)
		m_bgmVolume = mylib::Clamp(m_bgmVolume + delta, 0.0f, 1.0f);
	else if (m_currentOptionSelection == OptionSelection::SE)
		m_seVolume = mylib::Clamp(m_seVolume + delta, 0.0f, 1.0f);

	LoadJson json("Resources/Json/Music.json");

	// 音量を変更
	json.SetVolume("BGM", m_bgmVolume);
	json.SetVolume("SE", m_seVolume);

	// ファイルに保存
	json.SaveToFile("Resources/Json/Music.json");
}

