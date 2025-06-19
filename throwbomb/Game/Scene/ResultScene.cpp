/*
	@file	ResultScene.cpp
	@brief	���U���g�V�[���N���X
*/
#include "pch.h"
#include "ResultScene.h"
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
#include "Game/ResourceManager/ResourceManager.h"
#include "TitleScene.h"
#include <Game/Sound/Sound.h>
#include "Game/ResourceManager/ResourceManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
ResultScene::ResultScene()
	:
	m_commonResources{},
	m_projection{},
	m_isChangeScene{},
	m_position{},
	m_view{},
	m_clear{},
	m_currentSelection(Selection::Select),
	m_selectedScale(1.2f),
	m_unselectedScale(1.0f),
	m_replay(false),
	m_isSelect(false),
	m_scale{ 1.0f },
	m_elapsedTime{0},
	m_select{}

{

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
ResultScene::~ResultScene()
{
	Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void ResultScene::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// �X�v���C�g�o�b�`���쐬����
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	
	LoadResource(device);

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

	// ���̃C���X�^���X�̎擾�@�Đ�
	auto& sound = Sound::GetInstance();
	sound.Initialize();
	sound.PlayBGM(ResourceManager::getBGMPath("ResultBGM").c_str(), true);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void ResultScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// �㉺�L�[�őI����؂�ւ���
	switch (m_currentSelection)
	{
	case ResultScene::Selection::Select:
		if (kbTracker->pressed.Up)
		{
			m_currentSelection = Selection::Title;
		}
		else if (kbTracker->pressed.Down)
		{
			m_currentSelection = Selection::ReStart;
		}
		break;
	case ResultScene::Selection::ReStart:
		
		if (kbTracker->pressed.Up)
		{
			m_currentSelection = Selection::Select;
		}
		else if (kbTracker->pressed.Down)
		{
			m_currentSelection = Selection::Title;
		}
		break;
	case ResultScene::Selection::Title:
		if (kbTracker->pressed.Up)
		{
			m_currentSelection = Selection::ReStart;
		}
		else if (kbTracker->pressed.Down)
		{
			m_currentSelection = Selection::Select;
		}
		break;
	default:
		break;
	}
	

	// �X�y�[�X�L�[�������ꂽ��
	if (kbTracker->pressed.Space)
	{
		switch (m_currentSelection)
		{
		case ResultScene::Selection::Select:
			m_isSelect = true;
			break;
		case ResultScene::Selection::ReStart:
			m_replay = true;
			break;
		case ResultScene::Selection::Title:
			m_isChangeScene = true;;
			break;
		default:
			break;
		}
	}

	// �o�ߎ��Ԃ��X�V
	m_elapsedTime += elapsedTime;

	m_selectedScale = (sin(m_elapsedTime) + 2) / 3;
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void ResultScene::Render()
{

	//�e�N�X�`���[�̕`��
	this->TextureRender();
	
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void ResultScene::Finalize()
{
	Sound::GetInstance().Finalize();
}

//---------------------------------------------------------
// ���̃V�[��ID���擾����
//---------------------------------------------------------
IScene::SceneID ResultScene::GetNextSceneID() const
{
	// �V�[���ύX������ꍇ
	if (m_isChangeScene)
	{
		return IScene::SceneID::TITLE;
	  
	}
	if (m_replay)
	{
		return IScene::SceneID::PLAY;
	}
	if (m_isSelect)
	{
		return IScene::SceneID::SELECT;
	}

	// �V�[���ύX���Ȃ��ꍇ
	return IScene::SceneID::NONE;
}

void ResultScene::LoadResource(ID3D11Device1* device)
{
	// �摜�����[�h����
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Clear").c_str(),
			nullptr,
			m_clear.ReleaseAndGetAddressOf()
		)
	);

	// �^�C�g���w�i�摜�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Floor").c_str(),
			nullptr,
			m_background.ReleaseAndGetAddressOf()
		)
	);

	//�J�n�e�N�X�`���[�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("ReTry").c_str(),
			nullptr,
			m_reStart.ReleaseAndGetAddressOf()
		)
	);
	//�I���e�N�X�`���[�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("TitleButton").c_str(),
			nullptr,
			m_title.ReleaseAndGetAddressOf()
		)
	);
	//�Z���N�g�e�N�X�`���[�{�^�������[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Select").c_str(),
			nullptr,
			m_select.ReleaseAndGetAddressOf()
		)
	);
}

void ResultScene::TextureRender()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// �w�i�ƃX�e�[�W�N���A�̕`��i�ύX�Ȃ��j
	Vector2 post(0.0f, 0.0f);
	m_spriteBatch->Draw(m_background.Get(), post, nullptr, Colors::White, 0.0f, Vector2(0, 0), m_scale * 5.0f);
	Vector2 pos(Screen::CENTER_X, 50.0f);
	m_spriteBatch->Draw(m_clear.Get(), pos, nullptr, {1,1,1,1.0}, 0.0f, Vector2(320, 24), m_scale * 1.5);

	// �X�^�[�g�ƃG���h�I�����̕`��
	float restartScale = (m_currentSelection == Selection::ReStart) ? m_selectedScale : m_unselectedScale;
	float selectScale = (m_currentSelection == Selection::Select) ? m_selectedScale : m_unselectedScale;
	float titleScale = (m_currentSelection == Selection::Title) ? m_selectedScale : m_unselectedScale;

	Vector2 slectPos(Screen::CENTER_X, Screen::CENTER_Y );
	Vector2 restartPos(Screen::CENTER_X, Screen::CENTER_Y + 125);
	Vector2 titlePos(Screen::CENTER_X, Screen::CENTER_Y + 250);

	// �I�����̕`��
	m_spriteBatch->Draw(m_reStart.Get(), restartPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), restartScale);
	// �I�����̕`��
	m_spriteBatch->Draw(m_select.Get(), slectPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), selectScale);

	// �G���h�I�����̕`��
	m_spriteBatch->Draw(m_title.Get(), titlePos, nullptr, Colors::White , 0.0f,
		Vector2(140, 105), titleScale);

	m_spriteBatch->End();
}
