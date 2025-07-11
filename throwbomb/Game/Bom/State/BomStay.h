/*
  @file  BomStay.h
  @brief �{���̐^���ɒu���N���X
*/
#pragma once
#ifndef BOM_STAY_DEFINED
#define BOM_STAY_DEFINED

#include"Interface/IBomState.h"

class CommonResources;
class PlayerState;
class BomState;

class BomStay : public IBomState
{
public:
     // �R���X�g���N�^
     BomStay(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
     // �f�X�g���N�^
     ~BomStay();
     // ����������
     void Initialize(CommonResources* resources) override;
     // ���O�X�V����
     void PreUpdate();
     // �X�V����
     void Update(const float& elapsedTime) override;
     // ����X�V����
     void PostUpdate();
     // �`�悷��
     void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) override;
     // �㏈�����s��
     void Finalize() override;

private:
     //�Փ˔���
     void HitCheck(DirectX::BoundingBox boundingBox,const bool IsWall);

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // �v���C���[
    PlayerState* m_player; 
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // �{���X�e�[�g�i�e�j
    BomState* m_bomState;

    //���e�̃��f��
    std::unique_ptr<DirectX::Model> m_bomModel;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    //���x
    DirectX::SimpleMath::Vector3 m_velocity;
    //�����x
    DirectX::SimpleMath::Vector3 m_gravity;

    // �o�E���f�B���O�X�t�B�A
    DirectX::BoundingSphere m_boundingSphere;
};
#endif		//BOM_STAY_DEFINED