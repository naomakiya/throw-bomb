/*
	@file	UI.h
	@brief	ユーザーインターフェイスクラス
*/
#pragma once
#ifndef UI_DEFINED
#define UI_DEFINED
// 前方宣言
class CommonResources;
class HartUI;
class HollowHartUI;
class ButtonUI;
class Number;
class BomUI;

class UI
{
public:
	// ハートの数
	static const int HARTNUMBER;
public:
	// 数字の獲得
	Number* GetNumber() { return m_numberUI.get(); }
	// 入力レイアウトの定義（頂点シェーダ用）
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;
public:
	// コンストラクト
	UI();
	// デストラクト
	~UI();
	// 初期化
	void Initialize(CommonResources* resources);
	// 更新
	void Update();
	// 描画
	void Render(float elapsedTime);
	// 後処理
	void Finalize();
	// ハートの減少
	void CountHeart();
	// ハート・エンプティ
	bool AreHeartsEmpty() const;
private:
	// 共通リソース
	CommonResources* m_commonResources;
	// スプライトバッチ
	std::unique_ptr<DirectX::DX11::SpriteBatch> m_spriteBatch;
	// ハートUI
	std::vector<std::unique_ptr<HartUI>> m_hartsUI;
	// 空のハートUI
	std::vector < std::unique_ptr<HollowHartUI>> m_hollowHartsUI;
	// ボタンUI
	std::unique_ptr<ButtonUI> m_buttonUI;
	// 数字UI
	std::unique_ptr<Number> m_numberUI;
	// ボムUI
	std::unique_ptr<BomUI> m_bomUI;
};
#endif		// UI_DEFINED