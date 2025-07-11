/*
  @file  BomState.h
  @brief �{����ԃN���X
*/

#pragma once
#ifndef BOMSTATE_DEFINED
#define BOMSTATE_DEFINED
#include"Interface/IBomState.h"

// �O���錾
class CommonResources;
class BomExist;
class BomMovement;
class BomExplosion;
class BomStay;
class Wall;
class PlayerState;

class BomState : public IBomState
{

public:
	// �d��
	static constexpr float GRAVITY = -9.81f;
	// �{�����f���̑傫��
	static constexpr float BOMBMODELSIZE = 0.005f;
	// �����蔻��̑傫��
	static constexpr float BOUNDINGSPHERERADIUS = 0.5f;
public:
    // �p�x���擾����
    float GetAngle() const { return m_currentAngle; }
	// �p�x�̐ݒ�
	void SetAngle(const float& angel) { m_currentAngle = angel; }
    // �X�P�[�����擾����
    float GetScale() const { return m_scale; }
    // �X�P�[����ݒ肷��
    void SetScale(const float& scale) { m_scale = scale; }
    // �ʒu���擾����
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
	// �{���̏�Ԃ���ݒ肷��
	void SetBomPresent(IBomState::BomtPresent bomPresent) { m_bomPresent = bomPresent; }
	// ���݂̊l��
	bool GetExist() { return m_isExist; }
	// �����̐ݒ�
	void SetExist(const bool exist) { m_isExist = exist; }
	// ���x�̃Q�b�g
	DirectX::SimpleMath::Vector3 GetVelocity() { return m_velocity; }
	void SetVelocity(DirectX::SimpleMath::Vector3 velocity) { m_velocity = velocity; }
	// �o�E���f�B���O�X�t�B�A���擾����
	DirectX::BoundingSphere GetBoundingSphere() const { return m_BoundingSphere; }
	// �o�E���f�B���O�X�t�B�A��ݒ肷��
	void SetBoundingSphere(DirectX::BoundingSphere boundingSphere) { m_BoundingSphere = boundingSphere; }
	// �o�E���h��Ԃ��擾�i���˕Ԃ蒆���ǂ����j
	bool GetBounce() const  { return m_isBounce; } ;
	// �o�E���h��Ԃ�ݒ�i���˕Ԃ�J�n/��~�j
	void SetBounce(bool bounce) { m_isBounce = bounce; }
	// ���e�̎��Ԃ�ݒ�
	void SetExplosionTimer(float time) { m_explosionTimer = time; }
	// �������Ԃ��擾
	float GetExplosionTimer() const { return m_explosionTimer; }
	// �{���̏�Ԃ��擾����
	IBomState::BomtPresent GetBomPresent() const { return m_bomPresent; }
	// �v���C���[���l��
	PlayerState* GetPlayerState() { return m_player; }
    // �{���̐������擾����
	BomExist* GetBomExist() const { return m_bomExist.get(); }
    // �{���̓�����Ԃ��擾����
	BomMovement* GetBomMovement() const { return m_bomMovement.get(); }
    // �v���C���[�̃{��������Ԃ��擾����
	BomExplosion* GetBomExplosion() const { return m_bomExplosion.get(); }
	// �{���ɑ҂���Ԃ��擾
	BomStay* GetBomStay() const { return m_bomStay.get(); }

public:
	// �R���X�g���N�^
    BomState(PlayerState* m_Player, const std::vector<std::unique_ptr<Wall>>& wall) ;
	// �f�X�g���N�^
	~BomState();
	// ����������
	void Initialize(CommonResources* resources) override;
	// �V������ԂɑJ�ڂ���
	void ChangeState(IBomState* newBomState);
	// ���O�X�V
	void PreUpdate() {};
	// �X�V����
	void Update(const float& elapsedTime) override;
	// �`�悷��
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) override;
	// ����X�V
	void PostUpdate() {};
	// �㏈�����s��
	void Finalize()override;

private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	//��
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	//�v���C���[
	PlayerState* m_player;
	// ���݂̏�ԃN���X
	IBomState* m_currentState;
	// ���݂̏��
	BomtPresent m_bomPresent;
	// �Փ˔���p�X�t�B�A
	DirectX::BoundingSphere m_BoundingSphere;

	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	// ���x
	DirectX::SimpleMath::Vector3 m_velocity;

	// �����܂ł̎c�莞��
	float m_explosionTimer; 
	// �X�P�[��
	float m_scale;
	//���݂̊p�x
	float m_currentAngle;

	// ���݂̗L��
	bool m_isExist;
	// �����ς݂�
	bool m_isExploded;
	// ���˕Ԃ蒆��
	bool m_isBounce;

	// �{������
	std::unique_ptr<BomExist> m_bomExist;
	// �{���̈ړ�
	std::unique_ptr<BomMovement> m_bomMovement;
	// ����
	std::unique_ptr<BomExplosion> m_bomExplosion;
	// �u��
	std::unique_ptr<BomStay> m_bomStay;
};
#endif		// BOMSTATE_DEFINED