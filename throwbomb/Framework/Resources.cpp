#include "pch.h"
#include "Framework/Resources.h"

std::unique_ptr<Resources> Resources::m_resources = nullptr;

// Resourcesクラスのインスタンスを取得する
Resources* const Resources::GetInstance()
{
	if (m_resources == nullptr)
	{
		// Resourcesクラスのインスタンスを生成する
		m_resources.reset(new Resources());
	}
	// Resourcesクラスのインスタンスを返す
	return m_resources.get();
}

// リソースをロードする
void Resources::LoadResource()
{
	//// リソースディレクトリを設定する
	//m_graphics->GetFX()->SetDirectory(L"resources\\cmo");
	//// サッカーボールのモデルをロードする
	//m_soccerBallModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\SoccerBall.cmo", *m_graphics->GetFX());

	// リソースディレクトリを設定する
	//m_graphics->GetFX()->SetDirectory(L"resources\\sdkmesh");
	// プレーヤーモデルローダーフラグ
	/*DirectX::ModelLoaderFlags flags = DirectX::ModelLoader_Clockwise | DirectX::ModelLoader_IncludeBones;*/
	// SDKMESH形式のプレーヤーモデルをロードする
	/*m_soccerPlayerModel = DirectX::Model::CreateFromSDKMESH(m_device, L"resources\\sdkmesh\\Player.sdkmesh", *m_graphics->GetFX(), flags);*/
}
