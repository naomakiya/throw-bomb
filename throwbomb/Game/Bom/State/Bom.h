/*
  @file bom.h
  @brief �{���N���X
*/
#pragma once
#ifndef BOM_H
#define BOM_H

#include "Game/Scene/PlayScene.h"
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Effects.h>
#include <GeometricPrimitive.h>

// �O���錾
class CommonResources;
class PlayScene;
class Wall;
class PlayerState;

class Bom
{
	public:
	// �p�x���擾����
	float GetAngle() const { return m_currentAngle; }
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
	void SetExist(DirectX::SimpleMath::Vector3 playerPosition, const DirectX::SimpleMath::Vector3& playerForwardDirection);
	void SetExistStraight(DirectX::SimpleMath::Vector3 playerPosition, const DirectX::SimpleMath::Vector3& playerForwardDirection);
	//�����̊l��
	bool GetExist() {return m_exist; }
	bool GetMove() { return m_isMove; }
	bool GetExploded() { return m_exploded; }
	// �o�E���f�B���O�X�t�B�A���擾����
	DirectX::BoundingSphere GetBoundingSphere() const { return m_BoundingSphere; }
public:
	// �R���X�g���N�^
	 Bom(PlayScene* playScene, Graphics* graphics, const std::vector<std::unique_ptr<Wall>>& wall) ;
	//�f�X�g���N�^
	~Bom();
	//����������
	void Initialize(CommonResources* resources);
	//�X�V����
	void Update(const float& elapsedTime);
	//�`�悷��
	void Render(DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix projection) ;
	//���E�̋��̐���
	void DrawTurretBoundingSphere();
	//�㏈�����s��
	void Finalize();
	//���e�̋���
	void Movement(const float& elapsedTime);
	//���e�̋��E����\��
	void DrawExplosionBoundingSphere();
	//�����̋���
	void Explode(const float& elapsedTime);
	//���e�̊p�x��ς���
	void BomScaleChange(const float& elapsedTime, const DirectX::SimpleMath::Vector3& playerPosition);
	//�����蔻��
	void CheckHit(DirectX::BoundingBox boundingBox);
	//���e������
	void Hold(DirectX::SimpleMath::Vector3 playerPosition);
	//�o�E���h����
	void HandleCollision(const float& elapsedTime);

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

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//��
	const std::vector<std::unique_ptr<Wall>>& m_wall;
private:
	// ����
	float m_mass;
	// �p�x
	int m_angle;
	// �X�P�[��
	float m_scale;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	//���x
	DirectX::SimpleMath::Vector3 m_velocity;
	//�����x
	DirectX::SimpleMath::Vector3 m_gravity;
	//�J�E���g
	float m_cnt;
	//�����܂ł̎���
	float m_cntTime;
	//�����W��
	float m_restitution;
	// ����
	static constexpr float SPEED = 0.05f;
	//����
	bool m_exist;
	// �����܂ł̎���
	float m_timeToExplode;  
	// �����������ǂ���
	bool m_exploded; 
	//�����Ă�
	bool m_isMove = false;
	//�o�E���h���Ă�
	bool m_isBounce;
	// �o�E���h�̔����W��
	float m_bounceFactor;  
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