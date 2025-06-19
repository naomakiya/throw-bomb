/*
	@file	TitleScene.cpp
	@brief  �^�C�g���V�[���N���X
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "TitleScene.h"
#include "Game/Screen.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/UI/UI.h"
#include <Game/Sound/Sound.h>
#include "Libraries/MyLib/Utils.h"
#include <Framework/LoadJson.h>
#include "Game/TitleOp/TitleOp.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
TitleScene::TitleScene()
	:
	m_commonResources{nullptr},
	m_spriteBatch{ },
	m_fade{  },
	m_space{  },
	m_end{  },
	m_title{  },
	m_background{  },
	m_projection{  },
	m_view{ },
	m_position{},
	m_scale{ 1.0f },
	m_alpha{ 1.0f },
	m_elapsedTime{ 0.0f },
	m_isChangeScene{ false },
	m_visible{ true }, 
	m_IsFade{false},
	m_currentSelection(Selection::Start),
	m_currentOptionSelection(OptionSelection::BGM),
	m_selectedScale(1.2f),
	m_unselectedScale(1.0f),
	m_ui{},
	m_bgmVolume{0.0f},
	m_optionSelection{0},
	m_seVolume{0.0f},
	m_TOp{ nullptr },
	m_hasInput{ false }
{
	

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
TitleScene::~TitleScene()
{
	Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void TitleScene::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//CommonResources����̎g�p�f�[�^���o��
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	//�N���X�쐬
	CreateClass();

	//���\�[�X�̎�荞��
	LoadResource(device);

	// ��ʂ̃T�C�Y���擾����
	auto outputSize = m_commonResources->GetDeviceResources()->GetOutputSize();
	float screenWidth = static_cast<float>(outputSize.right - outputSize.left);
	float screenHeight = static_cast<float>(outputSize.bottom - outputSize.top);


	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, DirectX::SimpleMath::Vector3(0, 0, 10), DirectX::SimpleMath::Vector3::UnitY);

	m_projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		DirectX::XMConvertToRadians(45.0f), static_cast<float>(screenWidth) / static_cast<float>(screenHeight), 0.1f, 500.0f);

	LoadJson json("Resources/Json/Music.json");

	m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
	m_seVolume = json.GetJson()["SE"].value("Volume", 0.0f);

	// ���̃C���X�^���X�̎擾�@�Đ�
	auto& sound = Sound::GetInstance();
	sound.Initialize();
	sound.PlayBGM(ResourceManager::getBGMPath("TitleBGM").c_str(), true);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void TitleScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// �o�ߎ��Ԃ��X�V
	m_elapsedTime += elapsedTime;

	m_TOp->Update(elapsedTime);

	//�L�[�{�[�h����֌W
	if(!m_IsFade) KeyboardOperation();

	//�I��ł���V�[���̑傫���̕ύX
	m_selectedScale = (sin(m_elapsedTime) + 5.0f) / 5.0f;

	//1�b�ȏ�o������V�[����ς���
	if (m_fade->GetTime() >= 1.0f) {
		m_isChangeScene = true;
	}

	// �\����ԂɊ�Â��ăA���t�@�l��ݒ�
	m_alpha = m_visible ? 1.0f : 0.0f;

	//�t�F�C�h��ԂȂ�t�F�C�h�̎��Ԃ�i�߂�
	if (m_IsFade) {
		m_fade->Timer(elapsedTime);
	}
	
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void TitleScene::Render()
{
	//�e�N�X�`���[�̕`��
	TextureRender();

	//�I�v�V�����e�N�X�`���[�\��
	if(m_isOptionActive) RenderOption();

	if(!m_hasInput) m_TOp->Render(m_view, m_projection);

	//�t�F�C�h��Ԃ�
	if (m_IsFade){
		m_fade->Render(nullptr);
	}
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void TitleScene::Finalize()
{
	Sound::GetInstance().Finalize();
}

//---------------------------------------------------------
// ���̃V�[��ID���擾����
//---------------------------------------------------------
IScene::SceneID TitleScene::GetNextSceneID() const
{
	// �V�[���ύX������ꍇ
	if (m_isChangeScene)
	{
		return IScene::SceneID::SELECT;
	}

	// �V�[���ύX���Ȃ��ꍇ
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// �N���X�𐶐�
//---------------------------------------------------------
void TitleScene::CreateClass()
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto deviceResources = m_commonResources->GetDeviceResources();

	//�t�F�C�h�̐���
	m_fade = std::make_unique<Fade>();
	m_fade->Create(deviceResources);

	// ���[�U�[�C���^�[�t�F�C�X�̉q��
	m_ui = std::make_unique<UI>();
	m_ui->Initialize(m_commonResources);

	// �X�v���C�g�o�b�`���쐬����
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	m_TOp = std::make_unique<TitleOp>();
	m_TOp->Initialize(m_commonResources);
}

//---------------------------------------------------------
// ���\�[�X����荞��
//---------------------------------------------------------
void TitleScene::LoadResource(ID3D11Device1* device)
{
	LoadTexture(device, "TitleName", m_title);
	LoadTexture(device, "Strat", m_space);
	LoadTexture(device, "Finish", m_end);
	LoadTexture(device, "Floor", m_background);
	LoadTexture(device, "Option", m_option);
	LoadTexture(device, "Frame", m_optionFreame);
	LoadTexture(device, "BGM", m_bgm);
	LoadTexture(device, "SE", m_se);
	LoadTexture(device, "Box", m_box);
	LoadTexture(device, "VolumeBox", m_volumeBox);
	LoadTexture(device, "Close", m_close);
	LoadTexture(device, "Boms", m_bom);
}

//---------------------------------------------------------
// ���\�[�X�̓ǂݍ���
//---------------------------------------------------------
void TitleScene::LoadTexture(ID3D11Device1* device, const std::string& name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture)
{
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			ResourceManager::getTexturePath(name).c_str(),
			nullptr,
			texture.ReleaseAndGetAddressOf()
		)
	);
}

//---------------------------------------------------------
// �L�[�{�[�h����
//---------------------------------------------------------
void TitleScene::KeyboardOperation()
{
	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();
	
	//�ݒ��ʂ��J���Ă��鎞
	if (m_isOptionActive)
	{
		//�ݒ��ʂ̃L�[�{�[�h����
		HandleOptionInput(kbTracker.get());
	}
	else
	{
		//�^�C�g������
		HandleTitleMenuInput(kbTracker.get());
	}

	if (kbTracker->pressed.Up || kbTracker->pressed.Down || kbTracker->pressed.Space || kbTracker->pressed.Left || kbTracker->pressed.Right)
	{
		m_hasInput = true;
	}
	else
	{
		if (20.0f < m_elapsedTime && !m_isOptionActive)
		{
			m_hasInput = false;
			// �^�C�g����ʂ̏�Ԃ����Z�b�g����
			m_TOp->ResetBombs();
			// �^�C�}�[���[���ɂ���
			m_elapsedTime = 0.0f;
		}
	}
}

//---------------------------------------------------------
// �摜�̕`��
//---------------------------------------------------------
void TitleScene::TextureRender()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();

	// �X�v���C�g�̕`��J�n
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	// �w�i�ƃ^�C�g���̕`��i�ύX�Ȃ��j
	m_spriteBatch->Draw(m_background.Get(), Vector2(0, 0), nullptr, Colors::White, 0.0f,
		Vector2(0, 0), m_scale * 5.0f);

	// �^�C�g���摜�̕`��
	m_spriteBatch->Draw(m_title.Get(), Vector2(200.0f, 50.0f), nullptr, Colors::White, 0.0f,
		Vector2(0, 0), m_scale * 1.5f);

	// �X�^�[�g�ƃG���h�I�����̕`��
	float startScale = (m_currentSelection == Selection::Start) ? m_selectedScale : m_unselectedScale;
	float optionScale = (m_currentSelection == Selection::Option) ? m_selectedScale : m_unselectedScale;
	float endScale = (m_currentSelection == Selection::End) ? m_selectedScale : m_unselectedScale;

	// �X�^�[�g�̈ʒu
	Vector2 startPos(Screen::CENTER_X , Screen::CENTER_Y - 50);
	// �I�v�V�����̈ʒu
	Vector2 optionPos(Screen::CENTER_X, Screen::CENTER_Y + 100);
	// �G���h�̈ʒu
	Vector2 endPos(Screen::CENTER_X, Screen::CENTER_Y + 275);

	// �X�^�[�g�I�����̕`��
	m_spriteBatch->Draw(m_space.Get(), startPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(300, 50), startScale);
	// �I�v�V�����I�����̕`��
	m_spriteBatch->Draw(m_option.Get(), optionPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(200, 50), optionScale);
	// �G���h�I�����̕`��
	m_spriteBatch->Draw(m_end.Get(), endPos, nullptr, Colors::White * m_alpha, 0.0f,
		Vector2(200, 100), endScale);

	// �X�v���C�g�̕`��I��
	m_spriteBatch->End();
}

//---------------------------------------------------------
// �I�v�V�����̕\��
//---------------------------------------------------------
void TitleScene::RenderOption()
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();

	// �I���C���W�P�[�^�im_bom�j�̈ʒu�����߂ĕ`��
	Vector2 selectorOffset = Vector2(50, 80); // ���ɂ��炷 & ���S��肿�傢��

	// �X�v���C�g�̕`��J�n
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states->NonPremultiplied());

	//�i�I�v�V�����t���[��
	Vector2 framePos(Screen::CENTER_X - 300, 50);
	m_spriteBatch->Draw(m_optionFreame.Get(), framePos, nullptr, Colors::White, 0.0f,Vector2(0, 0), m_scale * 2.5f);

	//=== �e���ڂ̕`��ʒu�̃x�[�X ===
	const float itemSpacing = 180.0f;
	Vector2 itemPos = framePos + Vector2(0, 80); // �t���[���̒��ɃI�t�Z�b�g

	//=== �I��F�̒�` ===
	auto normalColor = Colors::White;
	auto selectedColor = Colors::Yellow;

	//=== BGM���� ===
	auto bgmColor = (m_currentOptionSelection == OptionSelection::BGM) ? selectedColor : normalColor;
	m_spriteBatch->Draw(m_bgm.Get(), itemPos, nullptr, bgmColor, 0.0f, Vector2(0, 0), 1.0f);

	// BGM�o�[�̕`��
	Vector2 bgmBarPos = itemPos + Vector2(300.0f, 30.0f);
	m_spriteBatch->Draw(m_box.Get(), bgmBarPos, nullptr, Colors::Gray, 0.0f, Vector2(0, 0), 1.0f);

	// �{�����[���{�b�N�X�̃\�[�X��`�i�����ŉ��ʂ�\���j
	RECT bgmSrc = { 0, 0, static_cast<LONG>(200 * m_bgmVolume), 100 };
	m_spriteBatch->Draw(m_volumeBox.Get(), bgmBarPos, &bgmSrc, Colors::Green, 0.0f, Vector2(0, 0), 1.0f);

	// BGM���I������Ă���ꍇ
	if (m_currentOptionSelection == OptionSelection::BGM) {
		m_spriteBatch->Draw(
			m_bom.Get(),
			itemPos + selectorOffset,
			nullptr,
			Colors::White,
			0.0f,
			Vector2(90, 90),
			m_selectedScale *0.5f
		);
	}
	//=== SE���� ===
	itemPos.y += itemSpacing;
	auto seColor = (m_currentOptionSelection == OptionSelection::SE) ? selectedColor : normalColor;
	m_spriteBatch->Draw(m_se.Get(), itemPos, nullptr, seColor, 0.0f, Vector2(0, 0), 1.0f);

	// SE�o�[�̕`��
	Vector2 seBarPos = itemPos + Vector2(300, 30 );
	m_spriteBatch->Draw(m_box.Get(), seBarPos, nullptr, Colors::Gray, 0.0f, Vector2(0, 0), 1.0f);

	RECT seSrc = { 0, 0, static_cast<LONG>(200 * m_seVolume), 100 };
	m_spriteBatch->Draw(m_volumeBox.Get(), seBarPos, &seSrc, Colors::Green, 0.0f, Vector2(0, 0), 1.0f);
	// SE���I������Ă���ꍇ
	if (m_currentOptionSelection == OptionSelection::SE) {
		m_spriteBatch->Draw(
			m_bom.Get(),
			itemPos  + selectorOffset,
			nullptr,
			Colors::White,
			0.0f,
			Vector2(90, 90),
			m_selectedScale * 0.5f
		);
	}

	//=== ���鍀�� ===
	itemPos.y += itemSpacing;
	auto closeColor = (m_currentOptionSelection == OptionSelection::END) ? selectedColor : normalColor;
	m_spriteBatch->Draw(m_close.Get(), itemPos + Vector2(50, 0), nullptr, closeColor, 0.0f, Vector2(0, 0), 1.0f);

	// END���I������Ă���ꍇ
	if (m_currentOptionSelection == OptionSelection::END) {
		m_spriteBatch->Draw(
			m_bom.Get(),
			itemPos + selectorOffset,
			nullptr,
			Colors::White,
			0.0f,
			Vector2(90, 90),
			m_selectedScale * 0.5f
		);
	}
	// �X�v���C�g�̕`��I��
	m_spriteBatch->End();
	
}

//---------------------------------------------------------
// �I�v�V��������
//---------------------------------------------------------
void TitleScene::HandleOptionInput(const DirectX::Keyboard::KeyboardStateTracker* tracker)
{
	if (tracker->pressed.Up) {
		if (m_currentOptionSelection == OptionSelection::SE)
			m_currentOptionSelection = OptionSelection::BGM;
		else if (m_currentOptionSelection == OptionSelection::END)
			m_currentOptionSelection = OptionSelection::SE;
	}

	if (tracker->pressed.Down) {
		if (m_currentOptionSelection == OptionSelection::BGM)
			m_currentOptionSelection = OptionSelection::SE;
		else if (m_currentOptionSelection == OptionSelection::SE)
			m_currentOptionSelection = OptionSelection::END;
	}

	if (tracker->pressed.Left) {
		AdjustVolume(-0.1f);
	}

	if (tracker->pressed.Right) {
		AdjustVolume(0.1f);
	}

	// ���ʓK�p
	Sound::GetInstance().SetVolume(m_bgmVolume);
	// �I�v�V�����I��
	if (tracker->pressed.Space && m_currentOptionSelection == OptionSelection::END) {
		m_isOptionActive = false;
		m_elapsedTime = 0.0f;
		m_currentSelection = Selection::Option;
	}
}

//---------------------------------------------------------
// �^�C�g������
//---------------------------------------------------------
void TitleScene::HandleTitleMenuInput(const DirectX::Keyboard::KeyboardStateTracker* tracker)
{
	if (tracker->pressed.Up) {
		m_currentSelection = static_cast<Selection>((static_cast<int>(m_currentSelection) - 1 + 3) % 3);
	}
	else if (tracker->pressed.Down) {
		m_currentSelection = static_cast<Selection>((static_cast<int>(m_currentSelection) + 1) % 3);
	}

	if (tracker->pressed.Space && m_hasInput) {
		switch (m_currentSelection) {
		case Selection::Start:
			m_IsFade = true;
			break;
		case Selection::Option:
			m_isOptionActive = true;
			break;
		case Selection::End:
			PostQuitMessage(0);
			break;
		}
	}
}

//---------------------------------------------------------
// �{�����[������
//---------------------------------------------------------
void TitleScene::AdjustVolume(float delta)
{
	if (m_currentOptionSelection == OptionSelection::BGM)
		m_bgmVolume = mylib::Clamp(m_bgmVolume + delta, 0.0f, 1.0f);
	else if (m_currentOptionSelection == OptionSelection::SE)
		m_seVolume = mylib::Clamp(m_seVolume + delta, 0.0f, 1.0f);

	LoadJson json("Resources/Json/Music.json");

	// ���ʂ�ύX
	json.SetVolume("BGM", m_bgmVolume);
	json.SetVolume("SE", m_seVolume);

	// �t�@�C���ɕۑ�
	json.SaveToFile("Resources/Json/Music.json");
}

