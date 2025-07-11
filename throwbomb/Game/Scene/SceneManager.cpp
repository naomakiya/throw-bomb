/*
	@file	SceneManager.cpp
	@brief	�V�[���}�l�[�W���N���X
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
// �R���X�g���N�^
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
// �f�X�g���N�^
//---------------------------------------------------------
SceneManager::~SceneManager()
{
	Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void SceneManager::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//���\�[�X�p�W�F�C�\���̃��[�h
	m_resourceManager = std::make_unique<ResourceManager>();
	m_resourceManager->loadFromJson("Resources/Json/Resource.json");
	// ���U���g�p�����[�^
	m_resultParamete = std::make_unique<ResultParamete>();
	ChangeScene(IScene::SceneID::TITLE);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void SceneManager::Update(float elapsedTime)
{
	m_currentScene->Update(elapsedTime);

	// �����p�ϐ��F���̃V�[��
	const IScene::SceneID nextSceneID = m_currentScene->GetNextSceneID();

	// �V�[����ύX���Ȃ��Ƃ�
	if (nextSceneID == IScene::SceneID::NONE) return;

	// �V�[����ύX����Ƃ�
	ChangeScene(nextSceneID);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void SceneManager::Render()
{
	m_currentScene->Render();
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void SceneManager::Finalize()
{
	DeleteScene();
}

//---------------------------------------------------------
// �V�[����ύX����
//---------------------------------------------------------
void SceneManager::ChangeScene(IScene::SceneID sceneID)
{
	DeleteScene();
	CreateScene(sceneID);
}

//---------------------------------------------------------
// �V�[�����쐬����
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
		assert(!"SceneManager::CreateScene::�V�[���������݂��܂���I");
		// no break
	}

	assert(m_currentScene && "SceneManager::CreateScene::���̃V�[������������܂���ł����I");

	m_currentScene->Initialize(m_commonResources);
}

//---------------------------------------------------------
// �V�[�����폜����
//---------------------------------------------------------
void SceneManager::DeleteScene()
{
	if (m_currentScene)
	{
		m_currentScene.reset();
	}
}