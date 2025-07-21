/*
	@file	HartUI.h
	@brief	�n�[�g�N���X
*/
#pragma once

// �O���錾
class CommonResources;

namespace DirectX{
	class DX11::SpriteBatch;
}

class HartUI
{
public:
	// �R���X�g���N�^
	HartUI();
	// �f�X�g���N�^
	~HartUI();
	// ������
	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	// �`��
	void Render(const int index);
	// �㏈��
	void Finalize();

private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �X�v���C�g�o�b�`
	DirectX::DX11::SpriteBatch* m_spriteBatch;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	// �e�N�X�`���̔����̑傫��
	DirectX::SimpleMath::Vector2 m_texSize;
};
