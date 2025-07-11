/*
  @file	 GameOver.h
  @brief �Q�[���I�[�o�[�N���X
*/
#pragma once
#include"Interface/IScene.h"

// �O���錾
class CommonResources;
class Sound;
class SceneManager;
class NumberUI;

class GameOver final :	public IScene
{
public:
	enum class Selection { NONE,Select, ReStart, Title };

public:
	// �R���X�^���g
	GameOver(SceneManager* sceneManager);
	// �f�X�g���N�^
	~GameOver() override;
	//�@������
	void Initialize(CommonResources* resources) override;
	// �X�V
	void Update(float elapsedTime)override;
	//�@�`��
	void Render() override;
	//�@����
	void Finalize() override;
	//�@���̃V�[���̊l��
	SceneID GetNextSceneID() const;

private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// ���\�[�X�}�l�[�W��
	SceneManager* m_sceneManager;
	// ��
	std::unique_ptr<Sound> m_sound;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// ����
	std::unique_ptr<NumberUI> m_numberUI;
	// �G�̐�
	std::unique_ptr<NumberUI> m_sumEnemyNumberUI;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_gameOver;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_select;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_reStart;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_selectBox;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_slash;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_remainingEnemy;
	// ���ݑI��ł��镨
	Selection m_currentSelection;
	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;
	// �r���[�s��i�Œ�J�����p�j
	DirectX::SimpleMath::Matrix m_view;
	//�Z���N�gBOX�ʒu
	DirectX::SimpleMath::Vector2 m_slectPos;
	// �g�̑傫���̕ύX
	float m_scale;
	// �V�[���`�F���W�t���O
	bool m_isChangeScene;
	// �e�N�X�`���̔����̑傫��
	DirectX::SimpleMath::Vector2 m_texCenter;

	float m_timer;
};