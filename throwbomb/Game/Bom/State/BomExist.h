/*
  @file BomExist.h
  @brief �{�������N���X
*/
#pragma once
#ifndef BOMEXIST_DEFINED
#define BOMEXIST_DEFINED

#include"Interface/IBomState.h"

class CommonResources;
class BomState;
class Wall;
class PlayerState;

class BomExist :public IBomState
{
public:
	// �R���X�g���N�^
	BomExist(BomState* bomState, PlayerState* player);
	//�f�X�g���N�^
	~BomExist();
	//����������
	void Initialize(CommonResources* resources)override;
	// ���O�X�V����
	void PreUpdate();
	//�X�V����
	void Update(const float& elapsedTime) override;
	// ����X�V����
	void PostUpdate();
	//�`�悷��
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) override;
	//�㏈�����s��
	void Finalize() override;
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �{�����
	BomState* m_bom;
	// �v���C��-
	PlayerState* m_player;

	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	// ���f��
	std::unique_ptr<DirectX::Model> m_bomModel;

	//���݂̓�����p�x
	float m_currentAngle;
	//��������
	float m_holdStartTime;
	// �v���C���[�̏�ɃX�|�[������{����Y���I�t�Z�b�g
	static constexpr float BomSpawnHeight = 1.5f;

	// �����̗L��
	bool m_isExist;
	// space�������Ă��邩
	bool m_isSpaceHeld;
	// �p�x��������
	bool m_isAngleIncreasing;
	//�{���������Ă��邩
	bool m_isHoldingBom;
	
	//�p�x�̑��x
    const float m_angleSpeed = 15.0f;
};
#endif		// BOMEXIST_DEFINED