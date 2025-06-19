/*
  @file  TitleOp.h
  @brief タイトルオープニングクラス
*/
#pragma once
#ifndef TITLEOP_DEFINED
#define TITLEOP_DEFINED

#include "Interface/IScene.h"
#include "Game/UI/Fade.h"

class CommonResources;
class Dithering;


class TitleOp
{
public:
	struct Bomb
	{
		DirectX::SimpleMath::Vector3 position;
		bool isExploded = false;
		bool hasChainedExplosion = false;
		float scale = 1.0f;
		float explodedTime = -1.0f;
		float explosionDuration = 1.5f;
	};

	struct ExplosionObject
	{
		DirectX::SimpleMath::Vector3 offset{};
		float scaleMultiplier = 1.0f;
		float startTime = 0.0f;
		float duration = 1.0f;
		bool active = true;
		int sourceBombIndex = -1;
	};
	struct ExplosionBufferData
	{
		DirectX::XMFLOAT4 explosionPositions[8];
	};
	//	データ受け渡し用コンスタントバッファ(送信側)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;	// ワールド行列
		DirectX::SimpleMath::Matrix		matView;	// ビュー行列
		DirectX::SimpleMath::Matrix		matProj;	// 射影行列
		DirectX::SimpleMath::Vector4	Diffuse;	// 拡散光のカラー
		DirectX::SimpleMath::Vector4	time;	    // 時間情報
	};
public:
	TitleOp();
	~TitleOp();
	void Initialize(CommonResources* resources);
	void Update(float elapsedTime);
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
	void Finalize();

	void ResetBombs();
private:
	void TriggerMainExplosionIfNeeded(Bomb& bomb);
	void TriggerChainExplosionIfNeeded(Bomb& bomb);
	void CreateMainExplosion(const DirectX::SimpleMath::Vector3& pos, int bombIndex);
	void CreateChainExplosions(const DirectX::SimpleMath::Vector3& center);
	void SetExplosions(const std::vector<DirectX::XMFLOAT4>& positions);
	void CreateShader();
	void LoadTexture(const wchar_t* path);
	void ShaderRender();
	DirectX::XMFLOAT2 WorldToUV(
		const DirectX::XMFLOAT3& worldPos,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& projection);
	private:
	CommonResources* m_commonResources;
	std::unique_ptr<Dithering> m_dithering;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	std::unique_ptr<DirectX::Model> m_bomModel;
	std::unique_ptr<DirectX::Model> m_explosionSphere;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBufferexp;

	std::vector<Bomb> m_bombs;
	std::vector<ExplosionObject> m_explosionObjects;

	DirectX::SimpleMath::Vector3 m_position{ 0.0f, 0.0f, -20.0f };
	DirectX::SimpleMath::Vector2 m_scale{ 1.0f, 1.0f };
	float m_elapsedTime = 0.0f;

	bool m_visible = true;
	bool m_IsFade = false;

	float m_bgmVolume = 1.0f;
	float m_seVolume = 1.0f;

private: //シャイダー用
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_CBuffer;  // コンスタントバッファ
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_texture;
	float m_time;

	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;
	// ビュー行列（固定カメラ用）
	DirectX::SimpleMath::Matrix m_view;
	
	float m_exprtime;
};

#endif		// TITLESCENE_DEFINED