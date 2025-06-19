/*
	@file	BomUI.h
	@brief	ボムUIクラス
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

	//	データ受け渡し用コンスタントバッファ(送信側)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Diffuse;
		DirectX::SimpleMath::Vector4    time;
	};
	//	関数
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;
public:
    ButtonUI();
	~ButtonUI();

	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	void Update();
	void Render();
	void Finalize();

	void CreateShader();

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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	// 数字テクスチャ
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_numberTexture;

	float m_overlayAlpha;

	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;
	//時間
	float m_time;
	//sicの動き
	float m_sin;

};
