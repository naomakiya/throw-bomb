/*
  @file EnemyTracking.h
  @brief �G�̒ǂ��������ԃN���X
*/
#pragma once
#ifndef ENEMYTRACKING_DEFINED
#define ENEMYTRACKING_DEFINED
#include "Interface/IEnemyState.h"

// �O���錾
class CommonResources;
class EnemyState;
class Wall;
class PlayerState;

class EnemyTracking : public IEnemyState
{
public:
    // �ǂƂ̏Փˏ���
    DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& wallBox, const DirectX::BoundingBox& entityBox);
    // �ʒu���擾����
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    // �ړI�n�̈ʒu��ݒ�
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //�G�̐����擾
    bool GetExist() const { return m_isExist; }
    //�G�̐����ݒ�
    void SetExist(const bool exist) { m_isExist = exist; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    //HP�����炷(�����Ŏg�p���Ȃ��j
    void HPDown() {};
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }

public:
    // �R���X�g���N�^
    EnemyTracking(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
    // �f�X�g���N�^
    ~EnemyTracking();
    // ������
    void Initialize(CommonResources* resources);
    // �O�X�V
    void PreUpdate();
    //�X�V
    void Update(const float& elapsedTime);
    //����X�V
    void PostUpdate();
    //�`��
    void Render(ID3D11DeviceContext* context,
        DirectX::CommonStates* states,
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection,
        const DirectX::Model& model);
    //����
    void Finalize();

private:
    // �ǐ�
    void Tracking(const float& elapsedTime);

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // �G
    EnemyState* m_enemy;
    // �v���C���[
    PlayerState* m_player;
    // ��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotate;
    // �G�̃o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
    // �G�̃o�E���f�B���O�X�t�B�A
    DirectX::BoundingSphere m_boundingSphere;
    // Texture�p�����o�[�i�e�N�X�`���[�ƃX�v���C�g�o�b�`�j
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_exclamationMarkTexture;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    // �G�̌��݈ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �o�ߎ���
    float m_time;
    // �ڕW�n�_�̃C���f�b�N�X
    unsigned int m_currentGoalNo;
    // �G���������Ă��邩
    bool m_isExist;
};
#endif  //ENEMYTRACKING_DEFINED