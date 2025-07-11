/*
  @file SelectScene.h
  @brief �Z���N�g�V�[���N���X
*/
#pragma once
#include"Interface/IScene.h"
#include"Game/UI/Fade.h"

// �O���錾
class CommonResources;
class SceneManager;
class Sound;

class SelectScene final :  public IScene
{
public:
	enum class Selection { NONE,ONE, TWO,THREE,TITLE };
public:
    // �R���X�g���N�g
	SelectScene(SceneManager* sceneManager);
	// �f�X�g���N�g
	~SelectScene() override;
	// ������
	void Initialize(CommonResources* resources) override;
	// �X�V
	void Update(float elapsedTime)override;
	// �`��
	void Render() override;
	// ��n��
	void Finalize() override;
	// ���̃V�[���̊l��
	SceneID GetNextSceneID() const;
private:
	// �N���X�̐���
	void ClassCreat();
	// ���\�[�X�ǂݍ��݁i�摜�e�N�X�`���Ȃǁj
	void LoadResource(ID3D11Device1* device);
	// �摜�`��
	void TextureRender();
	// �L�[�{�[�h���͂ɉ������I�����̍X�V
	void UpdateSelection();
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �V�[���Ǘ��N���X
	SceneManager* m_sceneManager;
	// ��
	std::unique_ptr<Sound> m_sound;

	// �`��֘A
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// UI
	std::unique_ptr<UI> m_ui;
	// �t�F�[�h�p��
	std::unique_ptr<Fade> m_fade;

	// �e�N�X�`���i�I�����A�w�i�Ȃǁj
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_one;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_two;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_three;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_selectBox;

	// �`��E�I���֘A�f�[�^
	// �e�N�X�`���̔����̑傫��
	DirectX::SimpleMath::Vector2 m_texCenter;
	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;
	// �r���[�s��i�Œ�J�����p�j
	DirectX::SimpleMath::Matrix m_view;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	//�Z���N�g�ʒu
	DirectX::SimpleMath::Vector2 m_slectPos;
	//�傫��
	DirectX::SimpleMath::Vector2 m_scale;
	// �I�����̂����X�P�[��
	DirectX::SimpleMath::Vector2 m_minScale;
	DirectX::SimpleMath::Vector2 m_maxScale;
	
	// ���݂̑I��ł��镨
	Selection m_currentSelection;

	// �\�����Ԋ֘A
	// �����x
	float m_alpha;
	// �o�ߎ��Ԃ��v��
	float m_elapsedTime; 
	// �g�̑傫���̕ύX
	float m_selectedScale;
	// �V�[���`�F���W�t���O
	bool m_isChangeScene;
	// �\����Ԃ�؂�ւ��邽�߂̃t���O
	bool m_visible;
	// �t�F�C�h�̃t���O
	bool m_IsFade;
};