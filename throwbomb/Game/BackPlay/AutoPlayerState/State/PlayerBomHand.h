/*
  @file �@PlayerBomHand.h
  @brief �v���C���{��������ԃN���X
*/
#pragma once
#ifndef PLAYERBOMHAND_DEFINED
#define PLAYERBOMHAND_DEFINED
#include"Interface/IPlayerState.h"

class CommonResources;
class PlayerState;
class Wall;
class Camera;

class PlayerBomHand :public IPlayerState
{
public:
	// �R���X�g���N�^
	PlayerBomHand(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
	//�f�X�g���N�^
	~PlayerBomHand();
	//������
	void Initialize(CommonResources* resources);
	// ���O�X�V����
	void PreUpdate();
	//�X�V����
	void Update(const float& elapsedTime);
	// ����X�V����
	void PostUpdate();
	//�`�悷��
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) ;
	//�㏈�����s��
	void Finalize();
	//�ړ�
	void Movement(const float& elapsedTime);
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	//�J����
	Camera* m_camera;
	//��
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	//�v���C�V�[��
	PlayerState* m_player;
	//�ʒu
	DirectX::SimpleMath::Vector3 m_postion;
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
	//��𓮂���
	float m_hand;
	//����
	float m_time;
};
#endif		// PLAYER_DEFINED