/*
   @file Interface/IPlayerState.h
   @brief �v���C���[�C���^�[�t�F�C�X�N���X
*/
#pragma once
#ifndef IPLAYERSTATE_DEFINED
#define IPLAYERSTATE_DEFINED
// �O���錾
class CommonResources;

class IPlayerState
{
public:
    virtual ~IPlayerState() = default;
    //����������
    virtual void Initialize(CommonResources* resources) = 0;
    //���O�X�V
    virtual void PreUpdate() = 0;
    //�X�V����
    virtual void Update(const float& elapsedTime) = 0;
    //����X�V
    virtual void PostUpdate() = 0;
    //�`�悷��
    virtual void Render(const DirectX::SimpleMath::Matrix& view, 
                        const DirectX::SimpleMath::Matrix& projection) = 0;
    //�㏈�����s�� 
    virtual void Finalize() = 0;
   
};

#endif