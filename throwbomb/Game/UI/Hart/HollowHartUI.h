/*
	@file	HollowHartUI.h
	@brief	空のハートクラス
*/
#pragma once
#ifndef HOLLOWHARTUI_DEFINED
#define HOLLOWHARTUI_DEFINED
// 前方宣言
class CommonResources;

namespace DirectX
{
	class DX11::SpriteBatch;
}

class HollowHartUI
{
public:
	// コンストラクタ
	HollowHartUI();
	// デストラクタ
	~HollowHartUI();
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
	// テクスチャの大きさ
	DirectX::SimpleMath::Vector2 m_texSize;
};
#endif  //HOLLOWHARTUI_DEFINED