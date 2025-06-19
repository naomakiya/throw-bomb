/*
  @file BomExist.cpp
  @brief �{��������ԃN���X
*/
#include "pch.h"
#include"Game/Bom/State/BomExist.h"
#include"Game/Bom/BomState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/DebugString.h"
#include"Game/Wall/Wall.h"
#include"Libraries/MyLib/InputManager.h"
#include"Game/Player/PlayerState.h"
#include"Game/Bom/State/BomMovement.h"
#include"Game/Bom/State/BomStay.h"
#include <Game/ResourceManager/ResourceManager.h>

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
BomExist::BomExist(BomState* bomState,PlayerState* player)
	: m_commonResources(nullptr)
	, m_bom(bomState)
	, m_player(player)
	, m_currentAngle(0.0f)
	, m_isExist(false)
	, m_isSpaceHeld(false)
	, m_isHoldingBom(false)
	, m_holdStartTime(0.0f)
	, m_isAngleIncreasing(false)
	, m_position()
{

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
BomExist::~BomExist()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void BomExist::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	// �{����3D���f����ǂݍ���
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Bom").c_str(), *fx);

}

//---------------------------------------------------------
// ���O�X�V����
//---------------------------------------------------------
void BomExist::PreUpdate()
{
	m_isExist = false;
	m_currentAngle = 0.0f;
	m_bom->SetBomPresent(BomState::Exist);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void BomExist::Update(const float& elapsedTime)
{
	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// �{�����܂����݂��Ă��Ȃ� �� ��ԕێ��m�F
	if (!m_isExist)
	{
		m_isExist = m_bom->GetExist();
		return;
	}

	// �{����ێ����F�v���C���[�̈ʒu�ɒǏ]
	m_position = m_player->GetPosition();
	m_position.y += BomSpawnHeight;
	m_bom->SetPosition(m_position);
	m_isHoldingBom = true;

	// ���������F�X�y�[�X�L�[���������u��
	if (kbTracker->pressed.Space && m_isHoldingBom)
	{
		m_isSpaceHeld = true;
		m_holdStartTime = 0.0f;
	}

	// �������s�F�X�y�[�X�L�[�������u��
	if (kbTracker->released.Space && m_isHoldingBom)
	{
		m_bom->ChangeState(m_bom->GetBomMovement());
		return;
	}

	// �u�������FX�L�[�ł��̏�ɐݒu
	if (kbTracker->pressed.X && m_isHoldingBom)
	{
		m_bom->ChangeState(m_bom->GetBomStay());
		return;
	}

	// �X�y�[�X�L�[�����������F�p�x�X�C���O�{������������
	if (m_isSpaceHeld)
	{
		m_holdStartTime += elapsedTime;

		// �p�x�X�C���O����
		if (m_isAngleIncreasing)
		{
			m_currentAngle -= elapsedTime * m_angleSpeed;
			if (m_currentAngle <= 0.0f) m_isAngleIncreasing = false;
		}
		else
		{
			m_currentAngle += elapsedTime * m_angleSpeed;
			if (m_currentAngle >= 90.0f) m_isAngleIncreasing = true;
		}

		// ��������10�b�������狭������
		if (m_holdStartTime >= 10.0f)
		{
			m_isSpaceHeld = false;
			m_bom->ChangeState(m_bom->GetBomMovement());
			return;
		}
	}

	// �����p�x�����L
	m_bom->SetAngle(m_currentAngle);
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void BomExist::PostUpdate()
{
	m_isSpaceHeld = false;
	m_bom->SetBomPresent(BomState::MOVEMENT);
	m_bom->SetAngle(m_currentAngle);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void BomExist::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	if (!m_isExist) return;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// ���[���h�s��쐬
	DirectX::SimpleMath::Matrix world =
		DirectX::SimpleMath::Matrix::CreateScale(0.005f) *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	// �{���̕`��
	m_bomModel->Draw(context, *states, world, view, projection);
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void BomExist::Finalize()
{
}
