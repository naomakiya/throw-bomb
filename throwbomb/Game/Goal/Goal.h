/*
  @file  Goal.h
  @brief �S�[���N���X
*/

#pragma once
#ifndef GAOL_DEFINED
#define GAOL_DEFINED
#include "GeometricPrimitive.h"

// �O���錾
class CommonResources;

class Goal
{
public:
	// �|�C���g���C�g�̍\����
	struct PointLight
	{
		DirectX::SimpleMath::Vector4 position;	// ���[���h���W
		DirectX::SimpleMath::Vector4 color;		// ���C�g�̐F
		DirectX::SimpleMath::Vector4 time;		// ���ԃp�����[�^
	};

public:
	// �X�P�[�����擾����
	float GetScale() const { return m_scale; }
	// �ʒu���擾����
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// �o�E���f�B���O�{�b�N�X���擾����
	DirectX::BoundingSphere GetBoundingSphere() const { return m_boundingSphere; }

public:
	// �R���X�g���N�^
	Goal(DirectX::SimpleMath::Vector3 pos);
	//�f�X�g���N�^
	~Goal();
	//����������
	void Initialize(CommonResources* resources);
	//�`�悷��
	void Render(ID3D11DeviceContext* context,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection) ;
	//�㏈�����s��
	void Finalize();

private:
	//���f���ǂݎ��
	void LoadModel(ID3D11Device* device);
	//�V�F�C�_�[�̓ǂݎ��
	void LoadShader(ID3D11Device* device);
	//�o�b�t�@�[�̐ݒ�
	void CreateConstanBuffer(ID3D11Device* device);

private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �S�[���̃��f��
	std::unique_ptr<DirectX::Model> m_model;
	//�s�N�Z���V�F�C�_�[
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	//�C���v�b�g���C���[
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//�萔�p�o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	// �o�E���f�B���O�{�b�N�X
	DirectX::BoundingSphere m_boundingSphere;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	// �X�P�[��
	float m_scale;
	//���Ԃ̍쐬
	float m_time;
};
#endif		// PLAYER_DEFINED