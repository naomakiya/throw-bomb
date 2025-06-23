/*
  @file�@PlayerState.h
  @brief �v���C����ԃN���X
*/

#pragma once
#ifndef PLAYERSTATE_DEFINED
#define PLAYERSTATE_DEFINED
#include"Interface/IPlayerState.h"

// �O���錾
class CommonResources;
class PlayerScene;
class PlayerIdling;
class PlayerBomHand;
class PlayerMovement;
class BomState;
class Wall;
class Camera;
class CollisionDebugRenderer;
class Shadow;
class Floor;

class PlayerState : public IPlayerState
{
public:
	// �p�x���擾����
	DirectX::SimpleMath::Quaternion GetAngle() const { return m_rotate; }
	// �p�x��ݒ肷��
	void SetAngle(const DirectX::SimpleMath::Quaternion& rotate) { m_rotate = rotate; }
	// �o�E���f�B���O�{�b�N�X���擾����
	DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
	// �ʒu���擾����
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// �ʒu��ݒ肷��
	void SetPosition(const DirectX::SimpleMath::Vector3& position){
		m_position = position;	m_boundingBox.Center = m_position;
	}
	// �{���X�e�[�g���擾����
	BomState* GetBomState(int index) { if (index >= 0 && index < m_bom.size()) { return m_bom[index].get(); } return nullptr;	}
	// ���݂̌����Ă���������擾����
	DirectX::SimpleMath::Vector3 GetForwardDirection() const {return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate);}
	// �G�Ƃ̓����蔻��̎擾
	bool GetHitEnemy() const { return m_isEnemyHit; }
	// �G�Ƃ̓����蔻��̐ݒ�
	void SetHitEnemy(const bool& isHitEnemy) {
		m_isEnemyHit = isHitEnemy;
		m_hitTimer = 0.0f;
		m_flashTimer = 0.0f;
	}
	// ���ݏ�Ԃ̎擾
	bool GetExist() const { return m_exist; }
	// �t���b�V����Ԃ̎擾
	bool GetFlashing() const { return m_isFlashing; }
	// �v���C���[�̃A�C�h�����O���擾����
	PlayerIdling* GetPlayerIdling() const { return m_playerIdling.get(); }
	// �v���C���[�̃{��������Ԃ��擾����
	PlayerMovement* GetPlayerMovement() const { return m_playerMovement.get(); }
	// �v���C���[�̃{��������Ԃ��擾����
	PlayerBomHand* GetPlayerBomHand() const { return m_playerBomHand.get(); }

public:
	// �R���X�g���N�^
	PlayerState(Camera* camera, const std::vector<std::unique_ptr<Wall>>& wall, DirectX::SimpleMath::Vector3 pos, std::vector<std::shared_ptr<Floor>>& floors);
	//�f�X�g���N�^
	~PlayerState();
	//����������
	void Initialize(CommonResources* resources);
	//�V������ԂɑJ�ڂ���
	void ChangeState(IPlayerState* newPlayerState);
	//�X�V����
	void Update(const float& elapsedTime);
	//�`�悷��
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
	//�㏈�����s��
	void Finalize();
	//�G�ƐG�ꂽ���̌���
	void EnemyHitFlashing(const DirectX::Model& model, const DirectX::FXMMATRIX world, const  DirectX::CXMMATRIX view, const  DirectX::CXMMATRIX projection);
	//�G�Ɠ���������
	void EnemyHit(const float& elapsedTime);

private:
	//���Ƃ̓����蔻��
	void CheckCollisionWithFloor();
	//�ǂƂ̓����蔻��
	void CheckCollisionWithWall();
	// �L�[�֌W
	void CheckKeyboard();
	//���O�X�V
	void PreUpdate();
	//����X�V
	void PostUpdate();
	//�e�̏�����
	void InitializeShadow();
	// �{���̏�����
	void InitializeBom();
	// �v���C���[�̏�Ԃ̏�����
	void InitializeStates();
	//�@�f�o�b�O�R���W���������_�[�̏�����
	void InitializeCollisionDebugRenderer();
	//�v���[���[�Ɣ��e�̔���
	void CheckBom();
	//���e�𓊂����������ĂȂ���
	bool IsThroeableBom();

public:
	//�{���̍ő吔
	static constexpr int BOMMAX = 3;
	// ����
	static constexpr float SPEED = 0.15f;
	// �d��
	static constexpr float GRVITY = 2.5f;
	// �v���C���[���f���̑傫��
	static constexpr float PLAYERMODLSCALE = 0.008;

private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// ��
	std::vector<std::shared_ptr<Floor>> m_floors;
	// �R���W�����f�o�b�N�����_�[
	std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;
	//��
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	//�J����
	Camera* m_camera;
	// �e�I�u�W�F�N�g
	std::unique_ptr<Shadow> m_shadow;
	//���݂̏��
	IPlayerState* m_currentState;
	// Player�̃��f��
	std::unique_ptr<DirectX::Model> m_playerModel;
	//�{��
	std::vector<std::unique_ptr<BomState>> m_bom;
	// �N�H�[�^�j�I���F���f���̉�]�𐧌䂷��
	DirectX::SimpleMath::Quaternion m_rotate;
	// �p�x
	DirectX::SimpleMath::Vector3 m_angle;
	// �o�E�f�B���O�{�b�N�X�̃X�P�[��
	DirectX::SimpleMath::Vector3 m_extentsScale;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	// �����_
	DirectX::SimpleMath::Vector3 m_target;
	//�߂�l�̒l�̊i�[
	DirectX::SimpleMath::Vector3 m_pushBack;
	//�o�E�f�B���O�{�b�N�X
	DirectX::BoundingBox m_boundingBox;
	// �{����������ꂽ���ǂ������Ǘ�����t���O
	bool m_bomExist[BOMMAX];
	//�������Ă��邩
	bool m_exist;
	// �ǂ̃q�b�g�t���O
	bool m_isHitWall;
	// ���̃q�b�g�t���O
	bool m_isHitFloor;
	//�G�Ƃ̃q�b�g
	bool m_isEnemyHit;
	// �Փˎ��̃^�C�}�[
	float m_hitTimer;
	// �_�ŏ��
	bool m_isFlashing;
	// �_�ł̂��߂̃^�C�}�[
	float m_flashTimer;
	//�����Ă���{���̃C���f�b�N�X
	int m_index;
	//��x�{���𓊂����������ĂȂ���
	bool m_isThrow;
	// �v���C���A�C�h�����O
	std::unique_ptr<PlayerIdling> m_playerIdling;
	// �v���C���[�X�{������
	std::unique_ptr<PlayerBomHand> m_playerBomHand;
	//�v���C���[�̓���
	std::unique_ptr< PlayerMovement> m_playerMovement;
};
#endif		// PLAYER_DEFINED