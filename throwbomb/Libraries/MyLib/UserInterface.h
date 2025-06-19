//--------------------------------------------------------------------------------------
// File: UserInterface.h
//
// ユーザーインターフェースクラス
//
//-------------------------------------------------------------------------------------

#pragma once

#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <Framework/DeviceResources.h>
#include <Framework/StepTimer.h>

//UIのアンカーポイントの列挙数
enum ANCHOR
{
	TOP_LEFT = 0,
	TOP_CENTER,
	TOP_RIGHT,
	MIDDLE_LEFT,
	MIDDLE_CENTER,
	MIDDLE_RIGHT,
    BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
};

class UserInterface
{
public:
	//データ受け渡し用コンスタントバッファ(送信側)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Vector4	windowSize;
	};
//変数
private:
	DX::DeviceResources* m_pDR;

	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;

	DX::StepTimer                           m_timer;
	// 入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	//コモンステート
	std::unique_ptr<DirectX::CommonStates> m_states;
	// テクスチャハンドル
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	Microsoft::WRL::ComPtr<ID3D11Resource> m_res;
	// 頂点シェーダ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	// ピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	// ジオメトリシェーダ
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

	int m_windowWidth , m_windowHeight;
	int m_textureWidth, m_textureHeight;

	DirectX::SimpleMath::Vector2 m_scale;
	DirectX::SimpleMath::Vector2 m_baseScale;
	DirectX::SimpleMath::Vector2 m_position;

	ANCHOR m_anchor;

	// レンダリング比率
	float m_renderRatio;
	// レンダリング比率オフセット
	float m_renderRatioOffset;

	// シェーダー内で使用されるパラメータ
	float m_smoothValue; // smoothValueの値
    //関数
public:
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;

	UserInterface();
	~UserInterface();

	void LoadTexture(const wchar_t* path);
		
	void Create(DX::DeviceResources* pDR, const wchar_t* texturePath, const wchar_t* vsPath, const wchar_t* gsPath, const wchar_t* psPath,
		DirectX::SimpleMath::Vector2 position, DirectX::SimpleMath::Vector2 scale, ANCHOR anchor);
	void SetScale(DirectX::SimpleMath::Vector2 scale);

	void Render();

	void SetWindowSize(const int &width,const int& height);

	DirectX::SimpleMath::Vector2 GetScale() { return m_scale; }
	DirectX::SimpleMath::Vector2 GetBaseScale() { return m_baseScale; }
	void SetPosition(DirectX::SimpleMath::Vector2 position);
	DirectX::SimpleMath::Vector2 GetPosition() { return m_position;}
	void SetAnchor(ANCHOR anchor);
	ANCHOR GetAnchor() { return m_anchor; }

	void SetRenderRatio(float ratio);
	float GetRenderRatio() { return m_renderRatio; }

	
	void SetRenderRatioOffset(float offset);
	float GetRenderRatioOffset() { return m_renderRatioOffset; }

	
	void CreateShader(const wchar_t* vsPath, const wchar_t* gsPath, const wchar_t* psPath);
};