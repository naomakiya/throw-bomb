/*
	@file	BomUI.h
	@brief	�{��UI�N���X
*/
#pragma once
#ifndef BOMUI_DEFINED
#define BOMUI_DEFINED

// �O���錾
class CommonResources;

namespace DirectX
{
	class DX11::SpriteBatch;
}

class BomUI
{
public:
	//	�f�[�^�󂯓n���p�R���X�^���g�o�b�t�@(���M��)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;	// ���[���h�ϊ��s��
		DirectX::SimpleMath::Matrix		matView;	// �r���[�ϊ��s��
		DirectX::SimpleMath::Matrix		matProj;	// �v���W�F�N�V�����ϊ��s��
		DirectX::SimpleMath::Vector4	Diffuse;	// �f�B�t���[�Y�J���[
		DirectX::SimpleMath::Vector4    time;		// ����
	};
public:
	// �R���X�g���N�^
    BomUI();
	// �f�X�g���N�^
	~BomUI();
	// ������
	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	// �`��
	void Render(float elapsedTime);
	// �㏈��
	void Finalize();
private:
	// �V�F�[�_�[�̍쐬
	void CreateShader();
	// �e�N�X�`���̓ǂݍ���
	void LoadTexture(const wchar_t* path);
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	//	�v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
	//�萔�o�b�t�@�[
	Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;
	// �X�v���C�g�o�b�`
	DirectX::DX11::SpriteBatch* m_spriteBatch;
	//	���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//	���_�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	//	�s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	//	�W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
	// �{���e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bomTexture;
	// �e�N�X�`���̔����̑傫��
	DirectX::SimpleMath::Vector2 m_texCenter;
	//����
	float m_time;
	//sic�̓���
	float m_sin;

};
#endif	//BOMUI_DEFINED