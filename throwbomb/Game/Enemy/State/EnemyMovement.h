/*
  @file  EnemyMovement.h
  @brief �G�̈ړ��N���X
*/

#pragma once
#ifndef ENEMY_MOVEMENT_DEFINED
#define ENEMY_MOVEMENT_DEFINED
#include "Interface/IEnemyState.h"
#include "Interface/IPatrolStrategy.h"

// �O���錾
class CommonResources;
class EnemyState;
class Wall;

class EnemyMovement :public IEnemyState
{
    
public:
    // �X�P�[�����擾����
    float GetScale() const { return m_scale; }
    // �ʒu���擾����
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    // �ړI�n�̈ʒu��ݒ�
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //�G�̐����擾
    bool GetExist() const { return m_exist; }
    //�G�̐����ݒ�
    void SetExist(const bool exist) { m_exist = exist; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // �o�E���f�B���O�{�b�N�X��ݒ肷��
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
    // �p�g���[���헪��ݒ�
    void SetPatrolStrategy(std::unique_ptr<IPatrolStrategy> strategy) {
        m_patrolStrategy = std::move(strategy);
    }

public:
    // �R���X�g���N�^
    EnemyMovement(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath);
    // �f�X�g���N�^
    ~EnemyMovement();
    // ����������
    void Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos);
    // ���O�X�V����
    void PreUpdate();
    // �X�V����
    void Update(const float&  elapsedTime);
    // ����X�V����
    void PostUpdate();
    // �`�悷��
    void Render(ID3D11DeviceContext* context,
         const DirectX::SimpleMath::Matrix& view,
         const DirectX::SimpleMath::Matrix& projection,
         const DirectX::Model& model);
    // �㏈�����s��
    void Finalize();
    // �������ꏊ�̐ݒ�
    void SetNearestGoal();
    //HP�����炷
    void HPDown();
    // �ł������S�[��
    int GetFurthestGoalFrom();
    // �o�E���f�B���O�n���̍X�V
    void UpdateBounding();
    // �p�g���[��
    void UpdateDirectionAndRotation(float elapsedTime);
    // �ǂ̓����蔻��
    void CheckWallCollision();
    // �{���̉���s��
    void CheckAvoidanceFromBombs();
    // �v���C���[�֌W�̓����蔻��
    void CheckCollisionWithPlayer();
    // �G�̓���
    void SyncWithEnemyState();

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // �G
    EnemyState* m_enemy;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // �ړ�����ʒu
    std::unique_ptr<IPatrolStrategy> m_patrolStrategy;

private:
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
	// ���[���h�}�g���b�N�X
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// �o�E���f�B���O�X�t�B�A
	DirectX::BoundingSphere m_boundingSphere;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotate; 
    // ���݂̃S�[���ԍ�
    unsigned int m_currentGoalNo;
    // ����o�H
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath; 
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    //�傫��
    float m_scale;
    //����
    bool m_exist;
    // �q�b�g�t���O
    bool m_isHit;
    // �ړ����x
    float m_moveSpeed;
    //���f���X�P�[��
    float m_modelScale;
    // �S�[�������������l
    float goalArrivalThreshold;

};
#endif		// ENEMYMOVEMENT_DEFINED