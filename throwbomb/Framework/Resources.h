#pragma once
#ifndef RESOURCES_DEFINED
#define RESOURCES_DEFINED
#include "Model.h"
#include "Graphics.h"

// Resources�N���X���`����
class Resources
{
public:
	// �u�T�b�J�[�{�[���v���f�����擾����
	DirectX::Model* GetSoccerBallModel() { return m_soccerBallModel.get(); }
	// �u�T�b�J�[�v���C���[�v���f�����擾����
	DirectX::Model* GetSoccerPlayerModel() { return m_soccerPlayerModel.get(); }

public:
	Resources(Resources&&) = default;
	Resources& operator= (Resources&&) = default;
	Resources& operator= (Resources const&) = delete;
	// �f�X�g���N�^
	~Resources() = default;
	// Resouces�N���X�̃C���X�^���X���擾����
	static Resources* const GetInstance();
	// ���\�[�X�����[�h����
	void LoadResource();

private:
	// �R���X�g���N�^
	Resources() noexcept
		:
		m_soccerBallModel{},
		m_soccerPlayerModel{}
	{
	}

private:
	// ���\�[�X
	static std::unique_ptr<Resources> m_resources;
	// �O���t�B�b�N�X
	Graphics* m_graphics = Graphics::GetInstance();
	// �f�o�C�X
	ID3D11Device* m_device = m_graphics->GetDeviceResources()->GetD3DDevice();

	// �T�b�J�[�{�[�����f��
	std::unique_ptr<DirectX::Model> m_soccerBallModel;
	// �v���[���[���f��
	std::unique_ptr<DirectX::Model> m_soccerPlayerModel;

};

#endif		// RESOURCES_DEFINED
