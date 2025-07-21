/*
  @file  Vase.h
  @brief ��N���X
*/

#pragma once
#ifndef VASE_DEFINED
#define VASE_DEFINED

// �O���錾
class CommonResources;
class CollisionDebugRenderer;

class Vase
{
public:
	// �ʒu���擾����
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// ��̐����擾
	bool GetExist() const { return m_isExist; }
	// ��̐����ݒ�
	void SetExist(const bool exist) { m_isExist = exist; }
	// �o�E���f�B���O�{�b�N�X���擾����
	DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
	// �o�E���f�B���O�X�t�B�A���擾����
	DirectX::BoundingSphere GetBoundingSphere() const { return m_boundingSphere; }

public:
	// �R���X�g���N�^
	Vase(DirectX::SimpleMath::Vector3 pos);
	//�f�X�g���N�^
	~Vase();
	//����������
	void Initialize(CommonResources* resources);
	//�`�悷��
	void Render(ID3D11DeviceContext* context,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection) ;
	//�㏈�����s��
	void Finalize();
	//��j���̏���
	void BreacVase();

private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �⃂�f��
	std::unique_ptr<DirectX::Model> m_vaseModel;
	// �f�o�b�O�����蔻��̕`��
	std::unique_ptr<CollisionDebugRenderer>  m_collisionDebugRenderer;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	// �o�E���f�B���O�{�b�N�X
	DirectX::BoundingBox m_boundingBox;
	// �o�E���f�B���O�X�t�B�A
	DirectX::BoundingSphere m_boundingSphere;
	//����
	bool m_isExist;
	
};
#endif		// PLAYER_DEFINED