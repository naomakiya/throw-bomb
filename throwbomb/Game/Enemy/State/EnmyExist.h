/*
  @file  EnemyExist.h
  @brief �G�̐�����ԃN���X
*/
#pragma once
#ifndef  ENEMYEXIST_DEFINED
#define  ENEMYEXIST_DEFINED
#include "Interface/IEnemyState.h"

// �O���錾
class CommonResources;
class EnemyState;
class Wall;

class EnemyExist :public IEnemyState
{
public:
    // �ʒu���擾����
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    // �ړI�n�̈ʒu��ݒ�(�����ł͎g�p�Ȃ��j
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //�G�̐����擾
    bool GetExist() const { return m_isExist; }
    //�G�̐����ݒ�
    void SetExist(const bool isexist) { m_isExist = isexist; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // �o�E���f�B���O�{�b�N�X���擾����
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }

public:
     // �R���X�g���N�^
     EnemyExist(EnemyState* enemyState);
     // �f�X�g���N�^
     ~EnemyExist();
     // ����������
     void Initialize(CommonResources* resources);
     // ���O�X�V����
     void PreUpdate();
     // �X�V����
     void Update(const float&  elapsedTime);
     // ����X�V����
     void PostUpdate();
     // �`�悷��
     void Render(ID3D11DeviceContext* context,
         DirectX::CommonStates* states,
         const DirectX::SimpleMath::Matrix& view,
         const DirectX::SimpleMath::Matrix& projection,
         const DirectX::Model& model);
     // �㏈�����s��
     void Finalize();
     //�@HP�������� (���̃N���X�ł͎g�p���Ȃ�)
     void HPDown() {};

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //���G�̓G
    EnemyState* m_enemy;
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotate;
    // ����
    float m_time;
    //����
    bool m_isExist;
};
#endif		// ENEMYEXIST_DEFINED