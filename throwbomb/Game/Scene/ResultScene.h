/*
  @file ResultScene.h
  @brief ���U���g�V�[���N���X
*/
#pragma once
#include"Interface/IScene.h"

// �O���錾
class CommonResources;

class ResultScene final :
	public IScene
{
public:
	enum class Selection { Select,ReStart, Title };
public:
	//�R���X�g���N�g
	ResultScene();
	//�f�X�g���N�g
	~ResultScene() override;
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
	//���\�[�X�����[�h����
	void LoadResource(ID3D11Device1* device);
	//�摜��`��
	void TextureRender();
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �X�v���C�g�o�b�`
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clear;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_select;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_reStart;
	// �e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
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
	// �傫��
	DirectX::SimpleMath::Vector2 m_scale;
	// ���ݑI��ł��镨
	Selection m_currentSelection;
	//�V�[���`�F���WPlayScene
	bool m_replay;
	//�V�[���`�F���WSelectScene
	bool m_isSelect;
	//�o�ߎ��Ԃ��v��
	float m_elapsedTime;
	float m_selectedScale;
	float m_unselectedScale;
	
};