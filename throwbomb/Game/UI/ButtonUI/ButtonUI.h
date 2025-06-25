/*
	@file	ButtonUI.h
	@brief	�{�^���Ɋւ���C���^�[�t�F�C�X�N���X
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
	//�R���X�g���N�g
	ButtonUI();
	//�f�X�g���N�g
	~ButtonUI();
	//������
	void Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch);
	//�`��
	void Render();
	//����
	void Finalize();
private:
	//���ʃR�������\�[�X
    CommonResources* m_commonResources;
	//�X�v���C�g�o�b�`
    DirectX::SpriteBatch* m_spriteBatch;
	//���e�N�X�`���[
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_arrowTexture;
	//�ڂ���UIA
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ButtonUITextureA;
	//�{�^��UID
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ButtonUITextureD;
	//�J�����e�N�X�`���[
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cameraUITexture;
	//�e�N�X�`���[�Z���^�[
    DirectX::SimpleMath::Vector2 m_texCenter;
	// �p�x
    float m_degree; 

};
