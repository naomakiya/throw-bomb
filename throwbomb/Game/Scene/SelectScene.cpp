/*
	@file	SelectScene.cpp
	@brief	�Z���N�g�V�[���N���X
*/
#include "pch.h"
#include "Framework/DeviceResources.h"
#include "SelectScene.h"
#include "Game/Screen.h"
#include "Libraries/MyLib/InputManager.h"
#include"Framework/CommonResources.h"
#include "Game/UI/UI.h"
#include"Game/ResourceManager/ResourceManager.h"
#include <Game/Sound/Sound.h>
#include "SceneManager.h"



//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
SelectScene::SelectScene(SceneManager* sceneManager)
	:
	m_commonResources{},
	m_spriteBatch{ },
	m_fade{  },
	m_one{  },
	m_two{  },
	m_three{},
	m_title{  },
	m_background{  },
	m_texCenter{  },
	m_projection{  },
	m_view{ },
	m_position{},
	m_scale{ 1.0f },
	m_alpha{ 1.0f },
	m_elapsedTime{ 0.0f },
	m_isChangeScene{},
	m_visible{ true }, // ������Ԃł͕\��
	m_IsFade{false},
	m_currentSelection(Selection::NONE),
	m_slectPos{0.0f,0.0f},
	m_ui{},
	m_sceneManager{ sceneManager },
	m_selectBox{},
	m_minScale{},
	m_maxScale{},
	m_selectedScale{0.0f}
{
	

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
SelectScene::~SelectScene()
{
	Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void SelectScene::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	//CommonResources����̎g�p�f�[�^���o��
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	
	
	// �N���X����
	ClassCreat();
	//���\�[�X�̎�荞��
	LoadResource(device);
	
	
	m_minScale = DirectX::SimpleMath::Vector2(1.9875f, 2.25f);
	m_maxScale = m_minScale * 1.1f; // 10%�g��i�����j

	// ��ʂ̃T�C�Y���擾����
	auto outputSize = m_commonResources->GetDeviceResources()->GetOutputSize();
	float screenWidth = static_cast<float>(outputSize.right - outputSize.left);
	float screenHeight = static_cast<float>(outputSize.bottom - outputSize.top);
	
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, DirectX::SimpleMath::Vector3(0, 0, 10), DirectX::SimpleMath::Vector3::UnitY);

	m_projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(45.0f), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 100.0f);

	// �V�[���ύX�t���O��������
	m_isChangeScene = false;
	m_currentSelection = Selection::ONE;
	Sound::GetInstance().Initialize();
	Sound::GetInstance().PlayBGM(ResourceManager::getBGMPath("TitleBGM").c_str(), true);
	Sound::GetInstance().SetVolume(0.1f);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void SelectScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// �o�ߎ��Ԃ��X�V
	m_elapsedTime += elapsedTime;

	UpdateSelection();

	float t = (sinf(m_elapsedTime * 2.0f) + 1.0f) * 0.1f; // 0.0 �` 1.0 �̎����I�ω�
	m_scale = m_minScale + (m_maxScale - m_minScale) * t;
	
	m_fade->Timer(elapsedTime);

	// �\����ԂɊ�Â��ăA���t�@�l��ݒ�
	m_alpha = m_visible ? 1.0f : 0.0f;
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void SelectScene::Render()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//�e�N�X�`���[�̕`��
	TextureRender();
	//�t�F�C�h��Ԃ�
	if (m_IsFade)
	{
		//�t�F�C�h�̕`��
		m_fade->Render(m_ui.get());

		//1�b�ȏ�o������V�[����ς���
		if (m_fade->GetTime() >= 2.0f)
		{
			m_isChangeScene = true;
		}
		
		return;
	}
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void SelectScene::Finalize()
{
	Sound::GetInstance().Finalize();
}

//---------------------------------------------------------
// ���̃V�[��ID���擾����
//---------------------------------------------------------
IScene::SceneID SelectScene::GetNextSceneID() const
{
	// �V�[���ύX������ꍇ
	if (m_isChangeScene)
	{
		switch (m_currentSelection)
		{
		case SelectScene::Selection::NONE:
			break;
		case SelectScene::Selection::ONE:
			m_sceneManager->SetSelectNumber(1);
			return IScene::SceneID::PLAY;			
			break;
		case SelectScene::Selection::TWO:
			m_sceneManager->SetSelectNumber(2);
			return IScene::SceneID::PLAY;
			break;
		case SelectScene::Selection::THREE:
			m_sceneManager->SetSelectNumber(3);
			return IScene::SceneID::PLAY;
			break;
		case SelectScene::Selection::TITLE:
			return IScene::SceneID::TITLE;
			break;
		default:
			break;
		}
		
	}

	// �V�[���ύX���Ȃ��ꍇ
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// �N���X�𐶐�����
//---------------------------------------------------------
void SelectScene::ClassCreat()
{
	auto deviceResources = m_commonResources->GetDeviceResources();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ���[�U�[�C���^�[�t�F�C�X
	m_ui = std::make_unique<UI>();
	m_ui->Initialize(m_commonResources);
	//�t�F�C�h�̐���
	m_fade = std::make_unique<Fade>();
	m_fade->Create(deviceResources);
	// �X�v���C�g�o�b�`���쐬����
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
}

//---------------------------------------------------------
// ���\�[�X����荞��
//---------------------------------------------------------
void SelectScene::LoadResource(ID3D11Device1* device)
{
	// 1�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("1").c_str(),
			nullptr,
			m_one.ReleaseAndGetAddressOf()
		)
	);
	// 2�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("2").c_str(),
			nullptr,
			m_two.ReleaseAndGetAddressOf()
		)
	);
	// 3�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("3").c_str(),
			nullptr,
			m_three.ReleaseAndGetAddressOf()
		)
	);
	// �^�C�g���w�i�摜�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("TitleButton").c_str(),
			nullptr,
			m_title.ReleaseAndGetAddressOf()
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
	// �^�C�g���w�i�摜�����[�h����
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath("Box").c_str(),
			nullptr,
			m_selectBox.ReleaseAndGetAddressOf()
		)
	);
}

void SelectScene::TextureRender()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// �w�i�ƃ^�C�g���̕`��
	Vector2 post(0.0f, 0.0f);
	m_spriteBatch->Draw(m_background.Get(), post, nullptr, Colors::White, 0.0f, Vector2(0, 0), 5.0f);
	Vector2 pos(250.0f, 50.0f);
	//m_spriteBatch->Draw(m_title.Get(), pos, nullptr, Colors::White, 0.0f, Vector2(0, 0), m_scale *1.5);

	Vector2 onePos(Screen::CENTER_X -400, Screen::CENTER_Y-50);
	Vector2 twoPos(Screen::CENTER_X, Screen::CENTER_Y - 50);
	Vector2 threePos(Screen::CENTER_X +400, Screen::CENTER_Y - 50);
	Vector2 endPos(Screen::CENTER_X , Screen::CENTER_Y + 200);

	//1�I�����̕`��
	m_spriteBatch->Draw(m_one.Get(), onePos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(639, 358), 0.3f);
	// 2�I�����̕`��
	m_spriteBatch->Draw(m_two.Get(), twoPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(637, 356), 0.3f);
	// 3�I�����̕`��
	m_spriteBatch->Draw(m_three.Get(), threePos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(637, 356), 0.3f);
	// �^�C�g���̕`��
	m_spriteBatch->Draw(m_title.Get(), endPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(140.0f, 105.0f), 1.5f);

	// �I�����̕`��
	m_spriteBatch->Draw(m_selectBox.Get(), m_slectPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(100.0f,50.0f), m_scale);
	m_spriteBatch->End();
}

void SelectScene::UpdateSelection()
{
	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();
	if (!m_IsFade)
	{
		switch (m_currentSelection)
		{
		case SelectScene::Selection::NONE:
			break;
		case SelectScene::Selection::ONE:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::THREE;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::TWO;
			}
			else if (kbTracker->pressed.Down)
			{
				m_currentSelection = Selection::TITLE;
			}

			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X - 400, Screen::CENTER_Y - 50 };
			break;
		case SelectScene::Selection::TWO:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::ONE;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::THREE;
			}
			else if (kbTracker->pressed.Down)
			{
				m_currentSelection = Selection::TITLE;
			}
			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X , Screen::CENTER_Y - 50 };
			break;
		case SelectScene::Selection::THREE:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::TWO;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::ONE;
			}
			else if (kbTracker->pressed.Down)
			{
				m_currentSelection = Selection::TITLE;
			}
			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X + 400, Screen::CENTER_Y - 50 };
			break;
		case SelectScene::Selection::TITLE:
			if (kbTracker->pressed.Left)
			{
				m_currentSelection = Selection::ONE;
			}
			else if (kbTracker->pressed.Right)
			{
				m_currentSelection = Selection::THREE;
			}
			else if (kbTracker->pressed.Up)
			{
				m_currentSelection = Selection::TWO;
			}
			m_slectPos = DirectX::SimpleMath::Vector2{ Screen::CENTER_X , Screen::CENTER_Y +200 };
			break;
		default:
			break;
		}

		// �X�y�[�X�L�[�������ꂽ��
		if (kbTracker->pressed.Space && !m_IsFade)
		{
			m_IsFade = true;
			m_fade->SetTime(0.0f);
		}
	}
}


