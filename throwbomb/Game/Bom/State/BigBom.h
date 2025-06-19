/*
  @file bom.h
  @brief �{���N���X
*/
#pragma once
#ifndef BIGBOM_DEFINED
#define BIGBOM_DEFINED
#include"Game/Scene/PlayScene.h"
#include <GeometricPrimitive.h>
#include <Game/Player/State/PlayerIdling.h>


// �O���錾
class CommonResources;
class PlayScene;

class BigBom
{
	public:
	// �p�x���擾����
	//float GetAngle() const { return m_angle; }
	// �p�x��ݒ肷��
	//void SetAngle(const float& angle) { m_angle = angle; }
	// �X�P�[�����擾����
	float GetScale() const { return m_scale; }
	// �X�P�[����ݒ肷��
	void SetScale(const float& scale) { m_scale = scale; }
	// �ʒu���擾����
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// �ʒu��ݒ肷��
	void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
	// �v���C�V�[�����擾����
	PlayScene* GetPlayScene() { return m_playScene; }
	//���e�̐����ݒ�
	void SetExist(DirectX::SimpleMath::Vector3 playerPosition);
	//�����̊l��
	bool GetExist() {return m_exist; }
	// �o�E���f�B���O�X�t�B�A���擾����
	DirectX::BoundingSphere GetBoundingSphere() const { return m_BoundingSphere; }
public:
	// �R���X�g���N�^
	 BigBom(PlayScene* playScene, Graphics* graphics, const std::vector<std::unique_ptr<Wall>>& wall) ;
	//�f�X�g���N�^
	~BigBom();
	//����������
	void Initialize(CommonResources* resources);
	//���E��
	DirectX::BoundingSphere CreateBoundingSphere(const float& radius);
	//�X�V����
	void Update(const float& elapsedTime);
	//�`�悷��
	void Render(DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix projection) ;
	//���E�̋��̐���
	void DrawTurretBoundingSphere();
	//�㏈�����s��
	void Finalize();
	//
	void DrawExplosionBoundingSphere();
	//�����̋���
	void Explode(const float& elapsedTime);

	void CheckHit(DirectX::BoundingBox boundingBox);
	private:
	// �O���t�B�b�N�X
	Graphics* m_graphics;
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �f�o�C�X�R���e�L�X�g
	ID3D11DeviceContext* m_context;
	// �v���C�V�[��
	PlayScene* m_playScene;
	// �W�I���g���b�N�v���~�e�B�u
	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;
	//����
	std::unique_ptr<DirectX::GeometricPrimitive> m_explosionSphere;
	// �o�E���f�B���O�X�t�B�A
	DirectX::BoundingSphere m_BoundingSphere;
	//��
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	// ����
	float m_mass;
	// �X�P�[��
	static float m_scale;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	//�����x
	DirectX::SimpleMath::Vector3 m_gravity;
	//�J�E���g
	float m_cnt;
	//�����܂ł̎���
	float m_cntTime;
	// ����
	static constexpr float SPEED = 0.05f;
	//����
	bool m_exist;
	// �����������ǂ���
	bool m_exploded; 
	//�����Ă�
	bool m_isMove = false;
	//�����̎���
	float m_explosionTime;  
	//���݂̊p�x
	float m_currentAngle;
	//�p�x�ύX���x
	float m_angleSpeed = 45;
	// �����̃X�t�B�A�R���C�_�[
	std::unique_ptr<DirectX::BoundingSphere> m_Sphere; 

};
#endif		// Bom_DEFINED