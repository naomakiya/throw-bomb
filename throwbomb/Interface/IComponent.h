#pragma once
#ifndef ICOMPONENT_DEFINED
#define ICOMPONENT_DEFINED
#include <SimpleMath.h>
#include <Model.h>
#include <Framework/CommonResources.h>

// IComponent�C���^�[�t�F�[�X���`����
class IComponent
{
public:
	// �m�[�h�ԍ����擾����
	virtual int GetNodeNumber() const = 0;
	// ���iID���擾����
	virtual int GetPartID() const = 0;
	// ���i�ԍ����擾����
	virtual int GetPartNumber() const = 0;
	// �e���擾����
	virtual IComponent* GetParent() const = 0;
	// ���݂̈ʒu���擾����
	virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;
	// ���݂̈ʒu��ݒ肷��
	virtual void SetPosition(const DirectX::SimpleMath::Vector3& curretPosition) = 0;
	// ���f�����擾����
	virtual DirectX::Model* GetModel() = 0;

public:
	// ���z�f�X�g���N�^
	virtual ~IComponent() = default;
	// ����������
	virtual void Initialize() = 0;
	// �X�V����
	virtual void Update(const DirectX::SimpleMath::Vector3& position) = 0;
	// �`�悷��
	virtual void Render() = 0;
	// �㏈�����s��
	virtual void Finalize() = 0;
};

#endif		// ICOMPONENT_DEFINED
