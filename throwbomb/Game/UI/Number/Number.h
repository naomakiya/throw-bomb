/*
	@file	Number.h
	@brief	数字クラス
*/
#pragma once
#ifndef NUMBER_DEFINED
#define NUMBER_DEFINED

// 前方宣言
namespace DX
{
	class DeviceResources;
}

class Number
{
public:
	//	データ受け渡し用コンスタントバッファ(送信側)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;	// ワールド行列
		DirectX::SimpleMath::Matrix		matView;	// ビュー行列
		DirectX::SimpleMath::Matrix		matProj;	// 射影行列
		DirectX::SimpleMath::Vector4	Diffuse;	// 拡散光
		DirectX::SimpleMath::Vector4    time;		// 時間情報
	};
	//時間の獲得
	float GetTime()const { return m_time; }
public:
	// コンストラクタ
	Number();
	// デストラクタ
	~Number();
	// テクスチャを読み込む
	void LoadTexture(const wchar_t* path);
	// リソースを作成
	void Create(DX::DeviceResources* pDR);
	// 描画
	void Render(float elapsedTime);
private:
	// シェーダを作成
	void CreateShader();
private:
	//	変数
	DX::DeviceResources* m_pDR;
	// コンスタントバッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;
	//	入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	//	コモンステート
	std::unique_ptr<DirectX::CommonStates> m_states;
	//	テクスチャハンドル
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_texture;
	//	テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;
	//	頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	//	ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	//	ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	//時間
	float m_time;
};
#endif	//NUMBER_DEFINED