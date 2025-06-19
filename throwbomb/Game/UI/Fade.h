/*
	@file	Fade.h
	@brief	�t�F�C�h�N���X
*/
#pragma once
#ifndef FADE_DEFINED
#define FADE_DEFINED

class UI;
namespace DX
{ 
	class DeviceResources;
}

class Fade
{
public:
	//	�f�[�^�󂯓n���p�R���X�^���g�o�b�t�@(���M��)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;	// ���[���h�s��
		DirectX::SimpleMath::Matrix		matView;	// �r���[�s��
		DirectX::SimpleMath::Matrix		matProj;	// �ˉe�s��
		DirectX::SimpleMath::Vector4	Diffuse;	// �g�U���̃J���[
		DirectX::SimpleMath::Vector4	time;	    // ���ԏ��
	};
	// �t�F�[�h�̎��Ԃ��擾
	float GetTime() { return m_time; }
	// �t�F�[�h�̎��Ԃ�ݒ�
	void SetTime(float time) { m_time = time; }
public:	
	// �R���X�g���N�^
	Fade();
	// �f�X�g���N�^
	~Fade();
	// �e�N�X�`�������[�h
	void LoadTexture(const wchar_t* path);
	// ���\�[�X���쐬
	void Create(DX::DeviceResources* pDR);
	// �`��
	void Timer(float elapsedTime);
	// �`��
	void Render(UI* ui);
private:
	// �V�F�[�_�̍쐬
	void CreateShader();
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
	//�e�N�X�`���[�ύX(�V�F�[�_�[���ɓn�����߂�float�^)
	float m_textureChage;

};
#endif	//FADE_DEFINED