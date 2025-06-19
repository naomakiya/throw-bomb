/*
	@file	Shadow.h
	@brief	�ۉe��\������N���X
*/
#pragma once
#ifndef SHADOW_DEFINED
#define SHADOW_DEFINED

class Shadow
{
public:
	Shadow();
	~Shadow() = default;

	// ����������
	void Initialize(
		ID3D11Device* device,
		ID3D11DeviceContext* context
	);

	// �`�悷��
	void Render(
		ID3D11DeviceContext* context,
		DirectX::CommonStates* states,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection,
		const DirectX::SimpleMath::Vector3& position,
		float radius = 0.7f
	);

private:
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

	// �v���~�e�B�u�o�b�`
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionTexture>> m_primitiveBatch;

	// �x�[�V�b�N�G�t�F�N�g
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	// �[�x�X�e���V���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	// �A���t�@�u�����h�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaBlendState;
};
#endif //SHADOW_DEFINED
