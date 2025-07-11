/*
  @file SceneManager.h
  @brief �V�[���}�l�[�W���[�N���X
*/
#pragma once
#include"Interface/IScene.h"

//�O���錾
class CommonResources;
class ResourceManager;
class ResultParamete;

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void Initialize(CommonResources* resources);
    void Update(float elapsedTime);
    void Render();
    void Finalize();

    void SetSelectNumber(int number) { m_number = number; }
    int GetSelectNumber() { return m_number; }
    ResultParamete* GetResultParamete() { return m_resultParamete.get(); }
private:
    void ChangeScene(IScene::SceneID sceneID);
    void CreateScene(IScene::SceneID sceneID);
    void DeleteScene();
    
private:
   //���݂̃V�[��
   std::unique_ptr<IScene> m_currentScene;
   //�R�������\�[�X
   CommonResources* m_commonResources;
   //���\�[�X
   std::shared_ptr<ResourceManager> m_resourceManager;
   //���U���g�p�����[�^
   std::unique_ptr<ResultParamete> m_resultParamete;
   // �Z���N�g�ԍ��̕ۑ�
   int m_number;
};