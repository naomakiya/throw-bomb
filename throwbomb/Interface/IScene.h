/*
   @file Game/IScene.h
   @brief シーンインターフェイスクラス
*/
#pragma once

// 前方宣言
class CommonResources;

class IScene
{

public:
     //シーンID
     enum class SceneID : unsigned int
     {
         NONE,
         TITLE,
         SELECT,
         PLAY,
         GAMEOVER,
         RESULT
     };

     virtual SceneID GetNextSceneID() const = 0;
public:
    virtual ~IScene() = default;
    virtual void Initialize(CommonResources* resources) = 0;
    virtual void Update(float elapsedTime) = 0;
    virtual void Render() = 0;
    virtual void Finalize() = 0;

   
};