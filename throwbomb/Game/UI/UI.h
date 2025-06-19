/*
	@file	UI.h
	@brief	���[�U�[�C���^�[�t�F�C�X�N���X
*/
#pragma once
#ifndef UI_DEFINED
#define UI_DEFINED
// �O���錾
class CommonResources;
class HartUI;
class HollowHartUI;
class ButtonUI;
class Number;
class BomUI;

class UI
{
public:
	// �n�[�g�̐�
	static const int HARTNUMBER;
public:
	// �����̊l��
	Number* GetNumber() { return m_numberUI.get(); }
	// ���̓��C�A�E�g�̒�`�i���_�V�F�[�_�p�j
	static const std::vector<D3D11_INPUT_ELEMENT_DESC> INPUT_LAYOUT;
public:
	// �R���X�g���N�g
	UI();
	// �f�X�g���N�g
	~UI();
	// ������
	void Initialize(CommonResources* resources);
	// �X�V
	void Update();
	// �`��
	void Render(float elapsedTime);
	// �㏈��
	void Finalize();
	// �n�[�g�̌���
	void CountHeart();
	// �n�[�g�E�G���v�e�B
	bool AreHeartsEmpty() const;
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::DX11::SpriteBatch> m_spriteBatch;
	// �n�[�gUI
	std::vector<std::unique_ptr<HartUI>> m_hartsUI;
	// ��̃n�[�gUI
	std::vector < std::unique_ptr<HollowHartUI>> m_hollowHartsUI;
	// �{�^��UI
	std::unique_ptr<ButtonUI> m_buttonUI;
	// ����UI
	std::unique_ptr<Number> m_numberUI;
	// �{��UI
	std::unique_ptr<BomUI> m_bomUI;
};
#endif		// UI_DEFINED