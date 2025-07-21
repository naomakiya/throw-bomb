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
#include <cassert>
#include "Framework/DeviceResources.h"
#include "Game/Screen.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "TitleScene.h"
#include "Game/Sound/Sound.h"
#include <Framework/LoadJson.h>
#include "SceneManager.h"
#include "Game/ResultParamete/ResultParamete.h"
#include "Game/UI/Number/NumberUI.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
ResultScene::ResultScene(SceneManager* sceneManager)
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
	m_select{},
	m_sceneManager{sceneManager},
	m_numberUI{},
	m_score{}

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

	// ���ʂ̓ǂݍ���
	LoadJson json("Resources/Json/Music.json");
	// BGM�̉��ʂ̐ݒ�
	float m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);

	m_sound = std::make_unique<Sound>();
	m_sound->Initialize();
	// BGM�̍Đ�
	m_sound->PlayBGM(ResourceManager::GetBGMPath("ResultBGM").c_str(), true);
	// ���ʂ̐ݒ�
	m_sound->SetVolume(m_bgmVolume);


	
	
	m_score = m_sceneManager->GetResultParamete()->GetScore();
	
	m_numberUI = std::make_unique<NumberUI>(static_cast<float>(m_score));
	m_numberUI->Create(m_commonResources->GetDeviceResources(),true);
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
		if (kbTracker->pressed.Left)
		{
			m_currentSelection = Selection::Title;
		}
		else if (kbTracker->pressed.Right)
		{
			m_currentSelection = Selection::ReStart;
		}
		m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X - 300, Screen::CENTER_Y + 250 };
		break;
	case ResultScene::Selection::ReStart:

		if (kbTracker->pressed.Left)
		{
			m_currentSelection = Selection::Select;
		}
		else if (kbTracker->pressed.Right)
		{
			m_currentSelection = Selection::Title;
		}
		m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X , Screen::CENTER_Y + 250 };
		break;
	case ResultScene::Selection::Title:
		if (kbTracker->pressed.Left)
		{
			m_currentSelection = Selection::ReStart;
		}
		else if (kbTracker->pressed.Right)
		{
			m_currentSelection = Selection::Select;
		}

		m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X + 300, Screen::CENTER_Y + 250 };
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

	m_scale = 0.125f * sinf(m_elapsedTime) + 1.5f;
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void ResultScene::Render()
{

	//�e�N�X�`���[�̕`��
	this->TextureRender();
	
	m_numberUI->Render(DirectX::SimpleMath::Vector3(0.0f, -0.2f, 0.0f));
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void ResultScene::Finalize()
{
	// ���̏I������
	m_sound->Finalize();
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
			ResourceManager::GetTexturePath("Clear").c_str(),
			nullptr,
			m_clear.ReleaseAndGetAddressOf()
		)
	);

	// �^�C�g���w�i�摜�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Floor").c_str(),
			nullptr,
			m_backGround.ReleaseAndGetAddressOf()
		)
	);

	//�J�n�e�N�X�`���[�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("ReTry").c_str(),
			nullptr,
			m_reStart.ReleaseAndGetAddressOf()
		)
	);
	//�I���e�N�X�`���[�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("TitleButton").c_str(),
			nullptr,
			m_title.ReleaseAndGetAddressOf()
		)
	);
	//�Z���N�g�e�N�X�`���[�{�^�������[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Select").c_str(),
			nullptr,
			m_select.ReleaseAndGetAddressOf()
		)
	);
	// �Z���N�gBox
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Box").c_str(),
			nullptr,
			m_selectBox.ReleaseAndGetAddressOf()
		)
	);
	// �X�R�A
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("Score").c_str(),
			nullptr,
			m_scoreTextur.ReleaseAndGetAddressOf()
		)
	);

	// A�X�R�A
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::GetTexturePath("A").c_str(),
			nullptr,
			m_aScore.ReleaseAndGetAddressOf()
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
	m_spriteBatch->Draw(m_backGround.Get(), post, nullptr, Colors::White, 0.0f, Vector2(0, 0), 1 * 5.0f);
	Vector2 pos(Screen::CENTER_X, 0.0f);
	m_spriteBatch->Draw(m_clear.Get(), pos, nullptr, {1,1,1,1.0}, 0.0f, Vector2(320, 24), 1 * 1.5);

	
	Vector2 restartPos(Screen::CENTER_X, Screen::CENTER_Y + 250);
	Vector2 slectPos(Screen::CENTER_X - 300, Screen::CENTER_Y + 250);
	Vector2 titlePos(Screen::CENTER_X + 300, Screen::CENTER_Y + 250);

	// �I�����̕`��
	m_spriteBatch->Draw(m_selectBox.Get(), m_slectPos, nullptr, Colors::Black, 0.0f,
		Vector2(100.0f, 50.0f), m_scale);
	// �I�����̕`��
	m_spriteBatch->Draw(m_reStart.Get(), restartPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), 1);
	// �I�����̕`��
	m_spriteBatch->Draw(m_select.Get(), slectPos, nullptr, Colors::White, 0.0f,
		Vector2(140, 105), 1);

	// �G���h�I�����̕`��
	m_spriteBatch->Draw(m_title.Get(), titlePos, nullptr, Colors::White , 0.0f,
		Vector2(140, 105), 1);
	// �X�R�A�̕���
	m_spriteBatch->Draw(m_scoreTextur.Get(), Vector2(Screen::CENTER_X, Screen::CENTER_Y - 50), nullptr, Colors::Black, 0.0f,
		Vector2(320.0f, 50.0f), 1);
	if (m_score > 20000)
	{
		m_spriteBatch->Draw(m_aScore.Get(), Vector2(Screen::CENTER_X- 500, Screen::CENTER_Y), nullptr, Colors::White, 0.0f,
			Vector2(5.0f, 50.0f), 2.5f);
	}
	m_spriteBatch->End();
}
