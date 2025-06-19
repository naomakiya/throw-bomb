/*
	@file	GameOver.cpp
	@brief	�Q�[���I�[�o�[�N���X
*/
#include "pch.h"
#include "GameOver.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include"Framework/Graphics.h"
#include <cassert>
#include "Framework/Graphics.h"
#include <Framework/Resources.h>
#include "Game/Screen.h"
#include <Game/ResourceManager/ResourceManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
GameOver::GameOver(ResourceManager* resourceManager)
	:
	m_commonResources{},
	m_projection{},
	m_isChangeScene{},
	m_position{},
	m_view{},
	m_clear{},
	m_resourceManager{resourceManager}

{

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
GameOver::~GameOver()
{
	// do nothing.
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void GameOver::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// �X�v���C�g�o�b�`���쐬����
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	// �摜�����[�h����
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/GameOver.png",
			nullptr,
			m_clear.ReleaseAndGetAddressOf()
		)
	);


	// �ȉ��A�e�N�X�`���̑傫�������߂遨�e�N�X�`���̒��S���W���v�Z����

	// �ꎞ�I�ȕϐ��̐錾
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
	D3D11_TEXTURE2D_DESC desc;
	Vector2 texSize;

	// �e�N�X�`���̏����擾����================================
	// �e�N�X�`����ID3D11Resource�Ƃ��Č���
	m_clear->GetResource(resource.GetAddressOf());
	// ID3D11Resource��ID3D11Texture2D�Ƃ��Č���
	resource.As(&tex2D);
	// �e�N�X�`�������擾����
	tex2D->GetDesc(&desc);

	// �e�N�X�`���T�C�Y���擾���Afloat�^�ɕϊ�����
	texSize.x = static_cast<float>(desc.Width);
	texSize.y = static_cast<float>(desc.Height);

	// �e�N�X�`���̒��S�ʒu���v�Z����
	m_texCenter = texSize / 2.0f;

	// �V�[���ύX�t���O������������
	m_isChangeScene = false;
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void GameOver::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	 //�X�y�[�X�L�[�������ꂽ��
	if (kbTracker->pressed.Space)
	{
	    m_isChangeScene = true;
	}


}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void GameOver::Render()
{
	auto states = m_commonResources->GetCommonStates();

	
	// �X�v���C�g�o�b�`�̊J�n�F�I�v�V�����Ń\�[�g���[�h�A�u�����h�X�e�[�g���w�肷��
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());
	// TRIDENT���S�̕`��ʒu�����߂�
	DirectX::SimpleMath::Vector2 pos = DirectX::SimpleMath::Vector2::Zero;
	pos = DirectX::SimpleMath::Vector2(360.0f, 180.0f);

	// TRIDENT���S��`�悷��
	m_spriteBatch->Draw(m_clear.Get(), pos);

	// �f�o�b�O����\������
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("Result");
	m_spriteBatch->End();
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void GameOver::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// ���̃V�[��ID���擾����
//---------------------------------------------------------
IScene::SceneID GameOver::GetNextSceneID() const
{
	// �V�[���ύX������ꍇ
	if (m_isChangeScene)
	{
		return IScene::SceneID::TITLE;
	  
	}

	// �V�[���ύX���Ȃ��ꍇ
	return IScene::SceneID::NONE;
}