/*
	@file	UI.cpp
	@brief	���[�U�[�C���^�[�t�F�C�X�N���X
*/
#include "pch.h"
#include "Game/UI/UI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/UI/Hart/HartUI.h"
#include"Game/UI/Hart/HollowHartUI.h"
#include"ButtonUI/ButtonUI.h"
#include"Number/Number.h"
#include"BomUI/BomUI.h"

// ��������n�[�g�̐�
const int UI::HARTNUMBER = 3;

///	<summary>
///	�C���v�b�g���C�A�E�g
///	</summary>
const std::vector<D3D11_INPUT_ELEMENT_DESC> UI::INPUT_LAYOUT =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::SimpleMath::Vector3) + sizeof(DirectX::SimpleMath::Vector4), D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
UI::UI()
	:
	m_commonResources{},
	m_spriteBatch{},
	m_hartsUI{},
	m_hollowHartsUI{},
	m_buttonUI{},
	m_numberUI{},
	m_bomUI{}
{
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
UI::~UI()
{
	
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void UI::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	// DirectX �̃f�o�C�X�R���e�L�X�g���擾
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto deviceResources = m_commonResources->GetDeviceResources();
	// �X�v���C�g�o�b�`���쐬����
	m_spriteBatch = std::make_unique<DirectX::DX11::SpriteBatch>(context);
	// �n�[�gUI���������i�v���C���[�̃��C�t�\���p�j
	for (int index = 0; index < HARTNUMBER; index++)
	{
		auto hart = std::make_unique<HartUI>();
		hart->Initialize(m_commonResources, m_spriteBatch.get());
		m_hartsUI.push_back(std::move(hart));
	}
	// �{�^��UI�̏������iUI�{�^���̕`��j
	m_buttonUI = std::make_unique<ButtonUI>();
	m_buttonUI->Initialize(resources, m_spriteBatch.get());
	// �����\���pUI�̏������i�J�E���g�p�j
	m_numberUI = std::make_unique<Number>();
	m_numberUI->Create(deviceResources);
	// ���eUI�̏������i���e�A�C�R���Ȃǂ̕`��j
	m_bomUI = std::make_unique<BomUI>();
	m_bomUI->Initialize(resources, m_spriteBatch.get());
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void UI::Update()
{
 
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void UI::Render(float elapsedTime)
{
	auto states = m_commonResources->GetCommonStates();

	// �X�v���C�g�o�b�`�̊J�n�F�I�v�V�����Ń\�[�g���[�h�A�u�����h�X�e�[�g���w�肷��
	m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());
	for (size_t index = 0; index < m_hollowHartsUI.size(); ++index)
	{
		m_hollowHartsUI[index]->Render(static_cast<int>(index));
	}
	// �n�[�gUI�i���C�t�\���j�̕`��
	for (size_t index = 0; index < m_hartsUI.size(); ++index)
	{
		m_hartsUI[index]->Render(static_cast<int>(index));
	}
	m_buttonUI->Render();
	m_bomUI->Render(elapsedTime);
	m_numberUI->Render(elapsedTime);

	// �X�v���C�g�o�b�`�̏I���
	m_spriteBatch->End();
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void UI::Finalize()
{
	m_numberUI.reset();
}

//---------------------------------------------------------
// �n�[�g�̐��̌���
//---------------------------------------------------------
void UI::CountHeart()
{
	// �n�[�g�̃��X�g����łȂ����`�F�b�N
	if (!m_hartsUI.empty()) 
	{
		// m_harts ���X�g�̍Ō�̗v�f���擾���A��������j�[�N�|�C���^�Ƃ��ĕێ�
		// std::move ���g���āAm_harts ���珊�L�����ړ�
		std::unique_ptr<HartUI> hart = std::move(m_hartsUI.back());

		// m_harts ���X�g�̍Ō�̗v�f���폜
		m_hartsUI.pop_back();

		// �V���� HollowHartUI �I�u�W�F�N�g���쐬���A���j�[�N�|�C���^�Ɋi�[
		auto hollowHart = std::make_unique<HollowHartUI>();

		// �V���� HollowHartUI �I�u�W�F�N�g��������
		hollowHart->Initialize(m_commonResources, m_spriteBatch.get());

		// �V���� HollowHartUI �I�u�W�F�N�g�� m_hollowHarts ���X�g�ɒǉ�
		m_hollowHartsUI.push_back(std::move(hollowHart));
	}
}

//---------------------------------------------------------
// �n�[�g������m�F
//---------------------------------------------------------
bool UI::AreHeartsEmpty() const
{
	// m_harts���󂩂ǂ������`�F�b�N����
	return m_hartsUI.empty();
}
