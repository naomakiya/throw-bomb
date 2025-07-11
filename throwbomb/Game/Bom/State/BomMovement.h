/*
  @file  BomMovement.h
  @brief �{���ړ��N���X
*/
#pragma once
#ifndef BOM_MOVEMENT_DEFINED
#define BOM_MOVEMENT_DEFINED

#include"Interface/IBomState.h"

class CommonResources;
class PlayerState;
class BomState;
class CollisionDebugRenderer;

class BomMovement : public IBomState
{
public:
     // �R���X�g���N�^
     BomMovement(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
     // �f�X�g���N�^
     ~BomMovement();
     // ����������
     void Initialize(CommonResources* resources) override;
     // ���O�X�V����
     void PreUpdate();
     void Projection(const DirectX::SimpleMath::Vector3& playerForwardDirection);
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
     //���e�̃o�E���h
     void HandleCollision();

public:
    // �����W��
    static constexpr float BOUNCEFACTOR = 0.8f;
    // �J�E���g�_�E��
    static constexpr float COUNTDOWNTIME = 3.5f;
    // �������x
    static constexpr float LAUNCHSPEED = 10.0f;
    // �n�ʂ̍���
    static constexpr float GROUNDHEIGHT = 0.7f;
    // �ŏ��̍���
    static constexpr float MINHEIGHT = 0.0f;

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // �v���C���[
    PlayerState* m_player;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // �{��
    BomState* m_bomState;
    // �R���W�����f�o�b�N�����_�[
    std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;

    //�{���̃��f��
    std::unique_ptr<DirectX::Model> m_bomModel;
    // �o�E���f�B���O�X�t�B�A
    DirectX::BoundingSphere m_boundingSphere;

    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    //���x
    DirectX::SimpleMath::Vector3 m_velocity;
    //�����x
    DirectX::SimpleMath::Vector3 m_gravity;
    

    //�o�E���h���Ă��邩
    bool m_isBounce;
};
#endif		// BOM_MOVEMENT_DEFINED