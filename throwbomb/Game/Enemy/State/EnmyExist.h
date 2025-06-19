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
    // �o�E���f�B���O�{�b�N�X���擾����
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
public:
     // �R���X�g���N�^
     EnemyExist(EnemyState* enemyState, 
         const std::vector<std::unique_ptr<Wall>>& wall);
     // �f�X�g���N�^
     ~EnemyExist();
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
     //�@HP�������� (���̃N���X�ł͎g�p���Ȃ�)
     void HPDown() {};
private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //���G�̓G
    EnemyState* m_enemy;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemy�̃��f��
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
	// ���[���h�}�g���b�N�X
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// �v���[���[���f��
	DirectX::Model* m_EnemyModel;
	// �o�E���f�B���O�X�t�B�A
	DirectX::BoundingSphere m_boundingSphere;
	////���f��
	std::unique_ptr<DirectX::Model> m_model;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotate;
    // �X�P�[��
    float m_scale;
    //����
    float m_time;
    //����
    bool m_exist;
 
};
#endif		// ENEMYEXIST_DEFINED