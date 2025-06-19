#pragma once
#ifndef IENEMYSTATE_DEFINED
#define IENEMYSTATE_DEFINED
#include "pch.h"
// IEnemy�C���^�[�t�F�[�X���`����
// 
// �O���錾
class CommonResources;

class IEnemyState
{
public:
	// �X�P�[�����擾����
	virtual float GetScale() const = 0;
	// �ʒu���擾����
	virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;
	// �ʒu��ݒ肷��
	virtual  void SetPosition(const DirectX::SimpleMath::Vector3& position) = 0;
	// �ړI�n�̈ʒu���擾
	virtual void SetPointPosition(DirectX::SimpleMath::Vector3& position) = 0;
	//�G�̐����擾
	virtual  bool GetExist() const = 0;
	//�G�̐����ݒ�
	virtual  void SetExist(const bool exist) = 0;
	// �o�E���f�B���O�{�b�N�X���擾����
	virtual DirectX::BoundingBox GetBoundingBox() const = 0;
	// ���݂̌����Ă���������擾����
	virtual DirectX::SimpleMath::Vector3 GetForwardDirection() const = 0;
public:
	// ���z�f�X�g���N�^
	virtual ~IEnemyState() = default;
	// ����������
	virtual void Initialize(CommonResources* resources,
		DirectX::SimpleMath::Vector3 pos) = 0;
	//���O�X�V
	virtual void PreUpdate() = 0;
	// �X�V����
	virtual void Update(const float& elapsedTime) = 0;
	//����X�V
	virtual void PostUpdate() = 0;
	// �`�悷��
	virtual void Render(ID3D11DeviceContext* context,
		                const DirectX::SimpleMath::Matrix& view,
		                const DirectX::SimpleMath::Matrix& projection,
						const DirectX::Model& model) = 0;
	// �㏈�����s��
	virtual void Finalize() = 0;
	//HP�����炷
	virtual void HPDown() = 0;
};
#endif