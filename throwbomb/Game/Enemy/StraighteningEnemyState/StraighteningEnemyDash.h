/*
  @file StraighteningEnemyDash.h
  @brief �_�b�V���G�N���X
*/
#pragma once
#ifndef  STRAIGHTENINGENEMYEXIST_DASH_DEFINED
#define  STRAIGHTENINGENEMYXIST_DASH_DEFINED
#include "Interface/IEnemyState.h"

// �O���錾
class CommonResources;
class StraighteningEnemyState;
class Wall;
class PlayerState;

class StraighteningEnemyDash :public IEnemyState
{
public:
    // �G�̐F�ω��̂̍\����
    struct StateColor
    {
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Vector4 color;
        DirectX::SimpleMath::Vector4 time;
    };
public:
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
    StraighteningEnemyDash(StraighteningEnemyState* enemyState,
                            const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
     // �f�X�g���N�^
     ~StraighteningEnemyDash();
     // ����������
     void Initialize(CommonResources* resources);
     // ���O�X�V����
     void PreUpdate();
     // �X�V����
     void Update(const float&  elapsedTime);
     DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& wallBox, const DirectX::BoundingBox& entityBox);
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
     //�V�F�C�_�[�̓ǂݎ��
     void ShaderLoad(ID3D11Device* device);
     //�o�b�t�@�[�̐ݒ�
     void CreateConstanBuffer(ID3D11Device* device);
     //�̗͂̌���
     void HPDown() {};
private:
    //�����x�N�g���v�Z
    void DirectionVectorCalculation();
private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //�ːi����G
    StraighteningEnemyState* m_enemy;
    //�v���C��
    PlayerState* m_player;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
	// ���[���h�}�g���b�N�X
	DirectX::SimpleMath::Matrix m_worldMatrix;
    //�N�H�[�^�I��
    DirectX::SimpleMath::Quaternion m_rotation;
	// �v���[���[���f��
	DirectX::Model* m_EnemyModel;
	//���f��
	std::unique_ptr<DirectX::Model> m_model;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotate;

    // Texture�p�����o�[�i�e�N�X�`���[�ƃX�v���C�g�o�b�`�j
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_exclamationMarkTexture;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    //��������
    DirectX::SimpleMath::Vector3 m_direction;
    //�_�b�V������܂ł̎���
    float m_dashTime;
    //����
    bool m_exist;
    // �q�b�g�t���O
    bool m_isHit;
 
};
#endif		// PLAYER_DEFINED