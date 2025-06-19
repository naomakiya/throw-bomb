/*
	@file	BomUI.h
	@brief	ボムUIクラス
*/
#pragma once
#ifndef BOMUI_DEFINED
#define BOMUI_DEFINED

// 前方宣言
class CommonResources;

namespace DirectX
{
	class DX11::SpriteBatch;
}

class BomUI
{
public:
	//	データ受け渡し用コンスタントバッファ(送信側)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;	// ワールド変換行列
		DirectX::SimpleMath::Matrix		matView;	// ビュー変換行列
		DirectX::SimpleMath::Matrix		matProj;	// プロジェクション変換行列
		DirectX::SimpleMath::Vector4	Diffuse;	// ディフューズカラー
		DirectX::SimpleMath::Vector4    time;		// 時間
	};
public:
	// コンストラクタ
    BomUI();
	// デストラクタ
	~BomUI();
	// 初期化
	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	// 描画
	void Render(float elapsedTime);
	// 後処理
	void Finalize();
private:
	// シェーダーの作成
	void CreateShader();
	// テクスチャの読み込み
	void LoadTexture(const wchar_t* path);
private:
	// 共通リソース
	CommonResources* m_commonResources;
	//	プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	//定数バッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;
	// スプライトバッチ
	DirectX::DX11::SpriteBatch* m_spriteBatch;
	//	入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	//	ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	//	ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	// ボムテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bomTexture;
	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;
	//時間
	float m_time;
	//sicの動き
	float m_sin;

};
#endif	//BOMUI_DEFINED