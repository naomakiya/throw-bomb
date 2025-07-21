/*
  @file  EnemySearch.h
  @brief �T���Ă����ԃN���X
*/
#pragma once
#ifndef ENEMYTSEARCH_DEFINED
#define ENEMYTSEARCH_DEFINED
#include "Interface/IEnemyState.h"

// �O���錾
class CommonResources;
class EnemyState;
class Wall;
class PlayerState;

class EnemySearch : public IEnemyState
{
public:
    // �R���X�g���N�^
    EnemySearch(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
    // �f�X�g���N�^
    ~EnemySearch();
    // ������
    void Initialize(CommonResources* resources);
    // �O�X�V
    void PreUpdate();
    //�X�V
    void Update(const float& elapsedTime);
    //����X�V
    void PostUpdate();
    //�`��
    void Render(ID3D11DeviceContext* context, DirectX::CommonStates* states, 
        const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection,
        const DirectX::Model& model);
    //����
    void Finalize();
    // �ǂƂ̏Փˏ���
    DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& wallBox, const DirectX::BoundingBox& entityBox);
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
    //HP�����炷
    void HPDown() {};
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // �G�̏�ԊǗ��I�u�W�F�N�g
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
    // �T���p�o�E���f�B���O�{�b�N�X
    DirectX::BoundingOrientedBox m_searchBoundingBox;

    // Texture�p�����o�[�i�e�N�X�`���[�ƃX�v���C�g�o�b�`�j
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_mysteryTexture;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    //�e�N�X�`���[�p�x
    float m_textureAngle;
    //�p�x�̍��v
    float m_totalTime;
    // ���[���h�s��
    DirectX::SimpleMath::Matrix m_worldMatrix;
    // �G�̌��݈ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �ڕW�n�_�̃C���f�b�N�X
    unsigned int m_currentGoalNo;
    // �o�ߎ���
    float m_time;
    // �G�̃X�P�[��
    float m_scale;
    //���f���̑傫��
    float m_modelScale;
    // �G���������Ă��邩
    bool m_exist;
};
#endif  //ENEMYTRACKING_DEFINED