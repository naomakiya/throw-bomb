/*
   @file Interface/IBomState.h
   @brief �{���̃C���^�[�t�F�C�X�N���X
*/
#pragma once
#ifndef IBOMSTATE_DEFINED
#define IBOMSTATE_DEFINED
// �O���錾
class CommonResources;

class IBomState
{
public:
    // ���e�̏��
    enum BomtPresent : short { Exist, MOVEMENT,STAY ,EXPLOSION };
public:
    virtual ~IBomState() = default;
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