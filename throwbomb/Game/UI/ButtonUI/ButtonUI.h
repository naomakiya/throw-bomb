/*
	@file	ButtonUI.h
	@brief	ボタンに関するインターフェイスクラス
*/
#pragma once

// 前方宣言
class CommonResources;

namespace DirectX
{
	class DX11::SpriteBatch;
}
// 一般的なシーンクラス
class ButtonUI
{
public:
	//コンストラクト
	ButtonUI();
	//デストラクト
	~ButtonUI();
	//初期化
	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	//描画
	void Render();
	//処理
	void Finalize();
private:
	//共通コモンリソース
    CommonResources* m_commonResources;
	//スプライトバッチ
    DirectX::SpriteBatch* m_spriteBatch;
	//矢印テクスチャー
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_arrowTexture;
	//ぼたんUIA
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ButtonUITextureA;
	//ボタンUID
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ButtonUITextureD;
	//カメラテクスチャー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cameraUITexture;
	//テクスチャーセンター
    DirectX::SimpleMath::Vector2 m_texCenter;
	// 角度
    float m_degree; 

};
