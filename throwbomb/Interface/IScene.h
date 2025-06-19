/*
   @file Game/IScene.h
   @brief �V�[���C���^�[�t�F�C�X�N���X
*/
#pragma once

// �O���錾
class CommonResources;

class IScene
{

public:
     //�V�[��ID
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