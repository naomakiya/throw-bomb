/*
  @file  BomState.cpp
  @brief �{���̏�ԃN���X
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include"Game/Bom/BomState.h"
#include"Game/Bom/State/BomExist.h"
#include"Game/Bom/State/BomMovement.h"
#include"Game/Bom/State/BomExplosion.h"
#include"Game/Bom/State/BomStay.h"
#include"Game/Wall/Wall.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
BomState::BomState(PlayerState* player,const std::vector<std::unique_ptr<Wall>>& wall)
	: m_commonResources{ nullptr }
	, m_wall{ wall }
	, m_player{ player }
	, m_currentState{ nullptr }
	, m_bomPresent{ IBomState::BomtPresent::Exist }
	, m_BoundingSphere{	}
	, m_position{ 0.0f }
	, m_velocity{ 0.0f }
	, m_explosionTimer{ 0.0f }
	, m_scale{ 1.0f }
	, m_currentAngle{ 0.0f }
	, m_isExist{ false }
	, m_isExploded{ false }
	, m_isBounce{ false }
{
	
	
	
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
BomState::~BomState()
{
	this->Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void BomState::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//���e�̐���
	m_bomExist = std::make_unique<BomExist>(this,m_player);
	m_bomExist->Initialize(m_commonResources);
	//���e�̓���
	m_bomMovement = std::make_unique<BomMovement>(this, m_player, m_wall);
	m_bomMovement->Initialize(m_commonResources);
	//���e�̔���
	m_bomExplosion = std::make_unique<BomExplosion>(this, m_wall, m_player);
	m_bomExplosion->Initialize(m_commonResources);
	//���e�̑҂�
	m_bomStay = std::make_unique<BomStay>(this, m_player, m_wall);
	m_bomStay->Initialize(m_commonResources);
	//���݂̏��
	m_currentState = m_bomExist.get();
}

//---------------------------------------------------------
// ��Ԃ̕ω�
//---------------------------------------------------------
void BomState::ChangeState(IBomState* newBomState)
{
	// �V�K�̏�ԑJ�ڑO�Ɏ���X�V���s��
	m_currentState->PostUpdate();
	// �V�K�̏�Ԃ����݂̏�Ԃɐݒ肷��
	m_currentState = newBomState;
	// �V�K�̏�ԑJ�ڌ�Ɏ��O�X�V���s��
	m_currentState->PreUpdate();
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void BomState::Update(const float& elapsedTime)
{
	m_explosionTimer -= elapsedTime;
	//���݂̏�Ԃ̍X�V
	m_currentState->Update(elapsedTime);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void BomState::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	//���݂̏�Ԃ̕`��
	m_currentState->Render(view, projection);
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void BomState::Finalize()
{
	m_bomExplosion.reset();
	m_bomExist.reset();
	m_bomMovement.reset();
	m_bomStay.reset();
}