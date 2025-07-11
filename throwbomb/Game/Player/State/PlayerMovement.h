/*
  @file �@PlayerMovement.h
  @brief �v���C���ړ���ԃN���X
*/

#pragma once
#ifndef PLAYE_RMOVEMENT_DEFINED
#define PLAYER_MOVEMENT_DEFINED
#include"Interface/IPlayerState.h"

class CommonResources;
class PlayerState;
class Camera;
class Wall;

class PlayerMovement :public IPlayerState
{
public:
	// �R���X�g���N�^
	PlayerMovement(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
	//�f�X�g���N�^
	~PlayerMovement();
	//����������
	void Initialize(CommonResources* resources) override;
	// ���O�X�V����
	void PreUpdate() override;
	//�X�V����
	void Update(const float& elapsedTime) override;
	// ����X�V����
	void PostUpdate() override;
	//�`�悷��
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) override;
	//�㏈�����s��
	void Finalize();

private:
	//�ړ�����
	void Movement(const float& elapsedTime);

public:
	// ����
	static constexpr float STARTSPEED = 1.0f;
	//�ő呬�x
	static constexpr float MAXSPEED = 5.0f;  
	// �����x
	const float acceleration = 0.15f;  

private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	//�v���C���[
	PlayerState* m_player;
	//��
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	//���݂̏��
	IPlayerState* m_currentState;
	//�J����
	Camera* m_camera;
    // �N�H�[�^�j�I���F���f���̉�]�𐧌䂷��
    DirectX::SimpleMath::Quaternion m_rotate;
    // �p�x
    DirectX::SimpleMath::Vector3 m_angle;
    // �X�P�[��
    DirectX::SimpleMath::Vector3 m_scale;
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
	//��𓮂���
	float m_hand;
	//�r�̐U��q
	float m_handAngle;
	//���x
	float m_vel;
	//����
	float m_time;
	
};
#endif		//  PLAYE_RMOVEMENT_DEFINED