/*
	@file	NumberUI.h
	@brief	�����N���X
*/
#pragma once
#ifndef NUMBER_DEFINED
#define NUMBER_DEFINED

// �O���錾
namespace DX
{
	class DeviceResources;
}

class NumberUI
{
public:
	//	�f�[�^�󂯓n���p�R���X�^���g�o�b�t�@(���M��)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;	// ���[���h�s��
		DirectX::SimpleMath::Matrix		matView;	// �r���[�s��
		DirectX::SimpleMath::Matrix		matProj;	// �ˉe�s��
		DirectX::SimpleMath::Vector4	Diffuse;	// �g�U��
		DirectX::SimpleMath::Vector4    time;		// ���ԏ��
	};
	//���Ԃ̊l��
	float GetTime()const { return m_time; }
public:
	// �R���X�g���N�^
	NumberUI(float score);
	// �f�X�g���N�^
	~NumberUI();
	// �e�N�X�`����ǂݍ���
	void LoadTexture(const wchar_t* path);
	// ���\�[�X���쐬
	void Create(DX::DeviceResources* pDR, bool isnumber);
	// �`��
	void Render(DirectX::SimpleMath::Vector3 pos);
private:
	// �V�F�[�_���쐬
	void CreateShader();

	void CreateShader5();
private:
	//	�ϐ�
	DX::DeviceResources* m_pDR;
	// �R���X�^���g�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;
	//	���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	�v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	//	�R�����X�e�[�g
	std::unique_ptr<DirectX::CommonStates> m_states;
	//	�e�N�X�`���n���h��
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_texture;
	//	�e�N�X�`���n���h��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;
	//	���_�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	//	�s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	//	�W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	//����
	float m_time;
};
#endif	//NUMBER_DEFINED