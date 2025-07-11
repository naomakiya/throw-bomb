/*
	@file	HartUI.h
	@brief	ハートクラス
*/
#pragma once

// 前方宣言
class CommonResources;

namespace DirectX
{
	class DX11::SpriteBatch;
}
// 一般的なシーンクラス
class HartUI
{
public:
	// コンストラクタ
	HartUI();
	// デストラクタ
	~HartUI();
	// 初期化
	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	// 描画
	void Render(const int index);
	// 後処理
	void Finalize();
private:
	// 共通リソース
	CommonResources* m_commonResources;
	// スプライトバッチ
	DirectX::DX11::SpriteBatch* m_spriteBatch;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texSize;
};
