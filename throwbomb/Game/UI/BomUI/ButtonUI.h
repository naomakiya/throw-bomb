/*
	@file	BomUI.h
	@brief	�{��UI�N���X
*/
#pragma once

// �O���錾
class CommonResources;

namespace DirectX
{
	class DX11::SpriteBatch;
}
// ��ʓI�ȃV�[���N���X
class ButtonUI
{
public:

	//	�f�[�^�󂯓n���p�R���X�^���g�o�b�t�@(���M��)
	struct ConstBuffer
	{
		DirectX::SimpleMath::Matrix		matWorld;
		DirectX::SimpleMath::Matrix		matView;
		DirectX::SimpleMath::Matrix		matProj;
		DirectX::SimpleMath::Vector4	Diffuse;
		DirectX::SimpleMath::Vector4    time;
	};
	//	�֐�
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;
public:
    ButtonUI();
	~ButtonUI();

	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	void Update();
	void Render();
	void Finalize();

	void CreateShader();

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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	// �����e�N�X�`��
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_numberTexture;

	float m_overlayAlpha;

	// �e�N�X�`���̔����̑傫��
	DirectX::SimpleMath::Vector2 m_texCenter;
	//����
	float m_time;
	//sic�̓���
	float m_sin;

};
