#pragma once
#ifndef RESOURCES_DEFINED
#define RESOURCES_DEFINED
#include "Model.h"
#include "Graphics.h"

// Resourcesクラスを定義する
class Resources
{
public:
	// 「サッカーボール」モデルを取得する
	DirectX::Model* GetSoccerBallModel() { return m_soccerBallModel.get(); }
	// 「サッカープレイヤー」モデルを取得する
	DirectX::Model* GetSoccerPlayerModel() { return m_soccerPlayerModel.get(); }

public:
	Resources(Resources&&) = default;
	Resources& operator= (Resources&&) = default;
	Resources& operator= (Resources const&) = delete;
	// デストラクタ
	~Resources() = default;
	// Resoucesクラスのインスタンスを取得する
	static Resources* const GetInstance();
	// リソースをロードする
	void LoadResource();

private:
	// コンストラクタ
	Resources() noexcept
		:
		m_soccerBallModel{},
		m_soccerPlayerModel{}
	{
	}

private:
	// リソース
	static std::unique_ptr<Resources> m_resources;
	// グラフィックス
	Graphics* m_graphics = Graphics::GetInstance();
	// デバイス
	ID3D11Device* m_device = m_graphics->GetDeviceResources()->GetD3DDevice();

	// サッカーボールモデル
	std::unique_ptr<DirectX::Model> m_soccerBallModel;
	// プレーヤーモデル
	std::unique_ptr<DirectX::Model> m_soccerPlayerModel;

};

#endif		// RESOURCES_DEFINED
