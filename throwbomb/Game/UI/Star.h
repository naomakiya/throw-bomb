//--------------------------------------------------------------------------------------
// File: Particle.h
//
// パーティクルクラス
// 未拡張の場合はPosition情報のみ
//
//-------------------------------------------------------------------------------------

#pragma once
#include "Framework/DeviceResources.h"
#include <list>
#include"StarUtility.h"

class CommonResources;

class Star
{
public:
		//	データ受け渡し用コンスタントバッファ(送信側)
		struct ConstBuffer
		{
			DirectX::SimpleMath::Matrix		matWorld;
			DirectX::SimpleMath::Matrix		matView;
			DirectX::SimpleMath::Matrix		matProj;
			DirectX::SimpleMath::Vector4	Diffuse;
		};
		void SetOn(bool on) { m_on = on; }
public:
	Star();
	~Star();

	void LoadTexture(const wchar_t* path);

	void Create(DX::DeviceResources* pDR);

	void Update(float elapsedTime,DirectX::SimpleMath::Vector3 pos);

	void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	void CreateShader();

	void CreateCircularParticles(int numParticles, float maxSpeed, float initialScale, float finalScale, float lifetime, DirectX::SimpleMath::Vector3 pos);
private:
		//	変数
		DX::DeviceResources* m_pDR;
		//位置
		DirectX::SimpleMath::Vector3 m_position;
		float m_timer;
		//ビルボード作成用変数
		DirectX::SimpleMath::Matrix m_Billboard;

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

		DirectX::SimpleMath::Matrix m_world;
		DirectX::SimpleMath::Matrix m_view;
		DirectX::SimpleMath::Matrix m_proj;

		DirectX::SimpleMath::Matrix m_billboard;

		std::vector<DirectX::VertexPositionColorTexture> m_vertices;
		std::list<StarUtility> m_starUtility;
		
		DirectX::SimpleMath::Vector3 m_cameraPosition;
		DirectX::SimpleMath::Vector3 m_cameraTarget;

		DX::StepTimer m_stepTimer;
		int m_textureIndex;

		bool m_on;


};