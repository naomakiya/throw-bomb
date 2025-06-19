/*
  @file	 GameOver.h
  @brief �Q�[���I�[�o�[�N���X
*/
#pragma once
#include"Interface/IScene.h"

// �O���錾
class CommonResources;
class ResourceManager;

class GameOver final :	public IScene
{
public:
	// �R���X�^���g
	GameOver(ResourceManager* resourceManager);
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
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	//���\�[�X�}�l�[�W��
	ResourceManager* m_resourceManager;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clear;
	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;
	// �r���[�s��i�Œ�J�����p�j
	DirectX::SimpleMath::Matrix m_view;
	// �ʒu
	DirectX::SimpleMath::Vector3 m_position;
	// �V�[���`�F���W�t���O
	bool m_isChangeScene;
	// �e�N�X�`���̔����̑傫��
	DirectX::SimpleMath::Vector2 m_texCenter;
};