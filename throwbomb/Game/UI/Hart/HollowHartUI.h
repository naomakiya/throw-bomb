/*
	@file	HollowHartUI.h
	@brief	��̃n�[�g�N���X
*/
#pragma once
#ifndef HOLLOWHARTUI_DEFINED
#define HOLLOWHARTUI_DEFINED
// �O���錾
class CommonResources;

namespace DirectX
{
	class DX11::SpriteBatch;
}

class HollowHartUI
{
public:
	// �R���X�g���N�^
	HollowHartUI();
	// �f�X�g���N�^
	~HollowHartUI();
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
	// �e�N�X�`���̑傫��
	DirectX::SimpleMath::Vector2 m_texSize;
};
#endif  //HOLLOWHARTUI_DEFINED