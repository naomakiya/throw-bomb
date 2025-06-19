/*
  @file Player.h
  @brief �v���C���N���X
*/
#pragma once
#ifndef ENEMYMOVEMENT_IDLING_DEFINED
#define ENEMYMOVEMENT_IDLING_DEFINED
#include "Interface/IEnemyState.h"
#include <random>
#include "Libraries/recastnavigation/Detour/Include/DetourNavMesh.h"
#include "Libraries/recastnavigation/Detour/Include/DetourNavMeshQuery.h"
#include"Libraries/MyLib/CollisionMesh.h"

// �O���錾
class CommonResources;
class EnemyState;
class Wall;
class mylib::CollisionMesh;

// A*�A���S���Y���ł̃p�X�i�o�H�j�^
using Path = std::vector<DirectX::SimpleMath::Vector3>;

class EnemyMovement :public IEnemyState
{
public:
    // �X�P�[�����擾����
    float GetScale() const { return m_scale; }
    // �X�P�[����ݒ肷��
    void SetScale(const float scale) { m_scale = scale; }
    // �ʒu���擾����
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //�G�̐����擾
    bool GetExist() const { return m_exist; }
    //�G�̐����ݒ�
    void SetExist(const bool exist) { m_exist = exist; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // �o�E���f�B���O�{�b�N�X���擾����
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
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
     void Update(const float&  elapsedTime,
         DirectX::SimpleMath::Vector3 target);
     // A*�A���S���Y���ł̈ړ�
     Path MoveTo(const DirectX::SimpleMath::Vector3& targetPosition);
     // ����X�V����
     void PostUpdate();
     // �`�悷��
     void Render(ID3D11DeviceContext* context,
         const DirectX::SimpleMath::Matrix& view,
         const DirectX::SimpleMath::Matrix& projection,
         const DirectX::Model& model);
     // �㏈�����s��
     void Finalize();
     void CheckHit(DirectX::BoundingBox boundingBox);
     void HPDown() {};
     void SetNearestGoal();
     bool PlayerCheckHit(DirectX::BoundingBox boundingBox);
private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //
    EnemyState* m_enemy;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemy�̃��f��
    //std::unique_ptr<DirectX::Model> m_enemyModel;
    // �R���W�������b�V��
    std::unique_ptr<mylib::CollisionMesh> m_collisionMesh;
private:
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
	// ���[���h�}�g���b�N�X
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// �o�E���f�B���O�X�t�B�A
	DirectX::BoundingSphere m_boundingSphere;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotation; 
    // ���݂̃S�[���ԍ�
    unsigned int m_currentGoalNo;
    // ����o�H
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath; 
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    //�傫��
    bool m_scale;
    //����
    bool m_exist;
    // �q�b�g�t���O
    bool m_isHit;

    bool m_isPaused;
    float m_pauseTimer;
    float m_pauseDuration;
    float m_timeUntilNextPause;
    // �V�[�h�l���擾����
    // std::mt19937 �ŗ��������������������
    std::mt19937 gen{ std::random_device{}() };
    std::uniform_real_distribution<float> m_pauseChance;
};
#endif		// PLAYER_DEFINED