/*
	@file	UI.cpp
	@brief	ユーザーインターフェイスクラス
*/
#include "pch.h"
#include "Game/UI/UI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/UI/Hart/HartUI.h"
#include"Game/UI/Hart/HollowHartUI.h"
#include"ButtonUI/ButtonUI.h"
#include"Number/Number.h"
#include"BomUI/BomUI.h"

// 生成するハートの数
const int UI::HARTNUMBER = 3;

///	<summary>
///	インプットレイアウト
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> UI::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
UI::UI()
	:
	m_commonResources{},
	m_spriteBatch{},
	m_hartsUI{},
	m_hollowHartsUI{},
	m_buttonUI{},
	m_numberUI{},
	m_bomUI{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
UI::~UI()
{
	
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void UI::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	// DirectX のデバイスコンテキストを取得
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto deviceResources = m_commonResources->GetDeviceResources();
	// スプライトバッチを作成する
	m_spriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(context);
	// ハートUIを初期化（プレイヤーのライフ表示用）
	for (int index = 0; index < HARTNUMBER; index++)
	{
		auto hart = std::make_unique<HartUI>();
		hart->Initialize(m_commonResources, m_spriteBatch.get());
		m_hartsUI.push_back(std::move(hart));
	}
	// ボタンUIの初期化（UIボタンの描画）
	m_buttonUI = std::make_unique<ButtonUI>();
	m_buttonUI->Initialize(resources, m_spriteBatch.get());
	// 数字表示用UIの初期化（カウント用）
	m_numberUI = std::make_unique<Number>();
	m_numberUI->Create(deviceResources);
	// 爆弾UIの初期化（爆弾アイコンなどの描画）
	m_bomUI = std::make_unique<BomUI>();
	m_bomUI->Initialize(resources, m_spriteBatch.get());
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void UI::Update()
{
 
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void UI::Render(float elapsedTime)
{
	auto states = m_commonResources->GetCommonStates();

	// スプライトバッチの開始：オプションでソートモード、ブレンドステートを指定する
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());
	for (size_t index = 0; index < m_hollowHartsUI.size(); ++index)
	{
		m_hollowHartsUI[index]->Render(static_cast<int>(index));
	}
	// ハートUI（ライフ表示）の描画
	for (size_t index = 0; index < m_hartsUI.size(); ++index)
	{
		m_hartsUI[index]->Render(static_cast<int>(index));
	}
	m_buttonUI->Render();
	m_bomUI->Render(elapsedTime);
	m_numberUI->Render(elapsedTime);

	// スプライトバッチの終わり
	m_spriteBatch->End();
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void UI::Finalize()
{
	m_numberUI.reset();
}

//---------------------------------------------------------
// ハートの数の減少
//---------------------------------------------------------
void UI::CountHeart()
{
	// ハートのリストが空でないかチェック
	if (!m_hartsUI.empty()) 
	{
		// m_harts リストの最後の要素を取得し、それをユニークポインタとして保持
		// std::move を使って、m_harts から所有権を移動
		std::unique_ptr<HartUI> hart = std::move(m_hartsUI.back());

		// m_harts リストの最後の要素を削除
		m_hartsUI.pop_back();

		// 新しい HollowHartUI オブジェクトを作成し、ユニークポインタに格納
		auto hollowHart = std::make_unique<HollowHartUI>();

		// 新しい HollowHartUI オブジェクトを初期化
		hollowHart->Initialize(m_commonResources, m_spriteBatch.get());

		// 新しい HollowHartUI オブジェクトを m_hollowHarts リストに追加
		m_hollowHartsUI.push_back(std::move(hollowHart));
	}
}

//---------------------------------------------------------
// ハートが空を確認
//---------------------------------------------------------
bool UI::AreHeartsEmpty() const
{
	// m_hartsが空かどうかをチェックする
	return m_hartsUI.empty();
}
