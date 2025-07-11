/*
	@file	SceneManager.cpp
	@brief	シーンマネージャクラス
*/
#include "pch.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "SelectScene.h"
#include "PlayScene.h"
#include "ResultScene.h"
#include "GameOver.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/ResultParamete/ResultParamete.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
SceneManager::SceneManager()
	:
	m_currentScene{},
	m_commonResources{},
	m_resourceManager{},
	m_resultParamete{},
	m_number{0}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
SceneManager::~SceneManager()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void SceneManager::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//リソース用ジェイソンのロード
	m_resourceManager = std::make_unique<ResourceManager>();
	m_resourceManager->loadFromJson("Resources/Json/Resource.json");
	// リザルトパラメータ
	m_resultParamete = std::make_unique<ResultParamete>();
	ChangeScene(IScene::SceneID::TITLE);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void SceneManager::Update(float elapsedTime)
{
	m_currentScene->Update(elapsedTime);

	// 説明用変数：次のシーン
	const IScene::SceneID nextSceneID = m_currentScene->GetNextSceneID();

	// シーンを変更しないとき
	if (nextSceneID == IScene::SceneID::NONE) return;

	// シーンを変更するとき
	ChangeScene(nextSceneID);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void SceneManager::Render()
{
	m_currentScene->Render();
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void SceneManager::Finalize()
{
	DeleteScene();
}

//---------------------------------------------------------
// シーンを変更する
//---------------------------------------------------------
void SceneManager::ChangeScene(IScene::SceneID sceneID)
{
	DeleteScene();
	CreateScene(sceneID);
}

//---------------------------------------------------------
// シーンを作成する
//---------------------------------------------------------
void SceneManager::CreateScene(IScene::SceneID sceneID)
{
	assert(m_currentScene == nullptr);

	switch (sceneID)
	{
	case IScene::SceneID::TITLE:
		m_currentScene = std::make_unique<TitleScene>();
		break;
	case IScene::SceneID::SELECT:
		m_currentScene = std::make_unique<SelectScene>(this);
		break;
	case IScene::SceneID::PLAY:
		m_currentScene = std::make_unique<PlayScene>(this);
		break;
	case IScene::SceneID::RESULT:
		m_currentScene = std::make_unique<ResultScene>(this);
		break;
	case IScene::SceneID::GAMEOVER:
		m_currentScene = std::make_unique<GameOver>(this);
		break;
	default:
		assert(!"SceneManager::CreateScene::シーン名が存在しません！");
		// no break
	}

	assert(m_currentScene && "SceneManager::CreateScene::次のシーンが生成されませんでした！");

	m_currentScene->Initialize(m_commonResources);
}

//---------------------------------------------------------
// シーンを削除する
//---------------------------------------------------------
void SceneManager::DeleteScene()
{
	if (m_currentScene)
	{
		m_currentScene.reset();
	}
}