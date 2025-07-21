/*
  @file Player.h
  @brief �v���C���N���X
*/
#pragma once
#ifndef  STRAIGHTENINGENEMYEXIST_IDLING_DEFINED
#define  STRAIGHTENINGENEMYXIST_IDLING_DEFINED
#include "Interface/IEnemyState.h"

// �O���錾
class CommonResources;
class StraighteningEnemyState;
class Wall;
class Star;

class StraighteningEnemyExist :public IEnemyState
{
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
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
public:
     // �R���X�g���N�^
    StraighteningEnemyExist(StraighteningEnemyState* enemyState,
                            const std::vector<std::unique_ptr<Wall>>& wall);
     // �f�X�g���N�^
     ~StraighteningEnemyExist();
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
     //�̗͂̌���
     void HPDown() {};
private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //�ːi����G
    StraighteningEnemyState* m_enemy;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemy�̃��f��
    std::unique_ptr<DirectX::Model> m_enemyModel;
    //��
    std::unique_ptr<Star> m_star;
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
	// ���[���h�}�g���b�N�X
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// �v���[���[���f��
	DirectX::Model* m_EnemyModel;
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
    // �q�b�g�t���O
    bool m_isHit;
 
};
#endif		// PLAYER_DEFINED