/*
  @file SceneManager.h
  @brief シーンマネージャークラス
*/
#pragma once
#include"Interface/IScene.h"

//前方宣言
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
   //現在のシーン
   std::unique_ptr<IScene> m_currentScene;
   //コモンリソース
   CommonResources* m_commonResources;
   //リソース
   std::shared_ptr<ResourceManager> m_resourceManager;
   //リザルトパラメータ
   std::unique_ptr<ResultParamete> m_resultParamete;
   // セレクト番号の保存
   int m_number;
};