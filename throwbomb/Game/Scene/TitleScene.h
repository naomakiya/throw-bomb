/*
  @file  TitleScene.h
  @brief �^�C�g���V�[���N���X
*/
#pragma once
#ifndef TITLESCENE_DEFINED
#define TITLESCENE_DEFINED

#include"Interface/IScene.h"
#include"Game/UI/Fade.h"

// �O���錾
class CommonResources;
class UI;
class TitleOp;

class TitleScene final :  public IScene
{
public:
	// �^�C�g���̑I����
	enum class Selection { Start, Option, End };
	// �ݒ�̑I����
	enum class OptionSelection{ BGM,SE,END};
public: 
    //�R���X�g���N�g
	TitleScene();
	//�f�X�g���N�g
	~TitleScene() override;
	//�C�j�V�����C�Y
	void Initialize(CommonResources* resources) override;
	//�X�V
	void Update(float elapsedTime)override;
	//�`��
	void Render() override;
	//��n��
	void Finalize() override;
	//���̃V�[���̊l��
	SceneID GetNextSceneID() const;
private:
	// �N���X�𐶐�����
	void CreateClass();
	//���\�[�X�����[�h����
	void LoadResource(ID3D11Device1* device);
	//���\�[�X�ǂݍ���
	void LoadTexture(ID3D11Device1* device, const std::string& name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture);
	// �L�[�{�[�h����
	void KeyboardOperation();
	//�摜��`��
	void TextureRender();
	//	�I�v�V����UI�̕\��
	void RenderOption();
	// �I�v�V��������
	void HandleOptionInput(const DirectX::Keyboard::KeyboardStateTracker* tracker);
	// �^�C�g������
	void HandleTitleMenuInput(const DirectX::Keyboard::KeyboardStateTracker* tracker);
	//�{�����[������
	void AdjustVolume(float delta);
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	//UI
	std::unique_ptr<UI> m_ui;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// �t�F�[�h�p��
	std::unique_ptr<Fade> m_fade;
	// �^�C�g��OP
	std::unique_ptr<TitleOp> m_TOp;

	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_space;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_end;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_optionFreame;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_option;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bgm;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_se;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_box;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_volumeBox;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_close;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bom;

	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;
	// �r���[�s��i�Œ�J�����p�j
	DirectX::SimpleMath::Matrix m_view;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	//�傫��
	DirectX::SimpleMath::Vector2 m_scale;
	//�����x
	float m_alpha;
	//�o�ߎ��Ԃ��v��
	float m_elapsedTime; 
	// �I�����ꂽ�X�P�[��
	float m_selectedScale;
	// ��I���X�P�[��
	float m_unselectedScale;
	//�V�[���`�F���W�t���O
	bool m_isChangeScene;
	// �\����Ԃ�؂�ւ��邽�߂̃t���O
	bool m_visible;
	//�t�F�C�h�̃t���O
	bool m_IsFade;
	bool m_hasInput;
	// ���ݑI��ł��镨
	Selection m_currentSelection;
	//���ݑI��ł���I�v�V����
	OptionSelection m_currentOptionSelection;

	bool m_isOptionActive = false;
	int m_optionSelection; // 0: BGM, 1: SE
	float m_bgmVolume;
	float m_seVolume;
	
};
#endif		// TITLESCENE_DEFINED