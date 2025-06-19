#include "pch.h"
#include "Framework/Resources.h"

std::unique_ptr<Resources> Resources::m_resources = nullptr;

// Resources�N���X�̃C���X�^���X���擾����
Resources* const Resources::GetInstance()
{
	if (m_resources == nullptr)
	{
		// Resources�N���X�̃C���X�^���X�𐶐�����
		m_resources.reset(new Resources());
	}
	// Resources�N���X�̃C���X�^���X��Ԃ�
	return m_resources.get();
}

// ���\�[�X�����[�h����
void Resources::LoadResource()
{
	//// ���\�[�X�f�B���N�g����ݒ肷��
	//m_graphics->GetFX()->SetDirectory(L"resources\\cmo");
	//// �T�b�J�[�{�[���̃��f�������[�h����
	//m_soccerBallModel = DirectX::Model::CreateFromCMO(m_device, L"resources\\cmo\\SoccerBall.cmo", *m_graphics->GetFX());

	// ���\�[�X�f�B���N�g����ݒ肷��
	//m_graphics->GetFX()->SetDirectory(L"resources\\sdkmesh");
	// �v���[���[���f�����[�_�[�t���O
	/*DirectX::ModelLoaderFlags flags = DirectX::ModelLoader_Clockwise | DirectX::ModelLoader_IncludeBones;*/
	// SDKMESH�`���̃v���[���[���f�������[�h����
	/*m_soccerPlayerModel = DirectX::Model::CreateFromSDKMESH(m_device, L"resources\\sdkmesh\\Player.sdkmesh", *m_graphics->GetFX(), flags);*/
}
