/*
  @file  PlayerIdling.h
  @brief �v���C���ҋ@��ԃN���X
*/
#pragma once
#ifndef PLAYER_IDLING_DEFINED
#define PLAYER_IDLING_DEFINED
#include"Interface/IPlayerState.h"

class CommonResources;
class PlayerState;
class Wall;

class PlayerIdling : public IPlayerState
{
public:
    // �R���X�g���N�^
    PlayerIdling(PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
    // �f�X�g���N�^
    ~PlayerIdling();
    // ����������
    void Initialize(CommonResources* resources);
    // ���O�X�V����
    void PreUpdate();
    // �X�V����
    void Update(const float& elapsedTime);
    // ����X�V����
    void PostUpdate();
    // �`�悷��
    void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
    // �㏈�����s��
    void Finalize();

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //�v���C�V�[��
    PlayerState* m_player;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Player�̊烂�f��
    std::unique_ptr<DirectX::Model> m_playerface;
    // Player�̃��f��
    std::unique_ptr<DirectX::Model> m_playerfaceBody;
    // Player�̍��胂�f��
    std::unique_ptr<DirectX::Model> m_playerHandL;
    // Player�̉E�胂�f��
    std::unique_ptr<DirectX::Model> m_playerHandR;
    // �N�H�[�^�j�I���F���f���̉�]�𐧌䂷��
    DirectX::SimpleMath::Quaternion m_rotate;
    // �o�E���f�B���O�X�t�B�A
    DirectX::BoundingSphere m_boundingSphere;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    //��𓮂���
    float m_hand;
    //����
    float m_time;
    //���f���̑傫��
    float m_modelScale;
    // ������
    float m_totalSeconds;
    // �A�j���[�V�����p�̕ϐ�
    float m_breathingFactor; 
};
#endif		// PLAYER_IDLING_DEFINED