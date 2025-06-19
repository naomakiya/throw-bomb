/*
  @file StraighteningEnemyExist.cpp
  @brief �G�l�~�[�ːi�N���X
*/
#include "pch.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemyExist.h"
#include "Game/Enemy/StraighteningEnemyState/StraighteningEnemySearch.h"
#include"Game/Enemy/StraighteningEnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/Resources.h"
#include"Game/Wall/Wall.h"
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <DirectXCollision.h>
#include"Interface/IEnemyState.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include "Game/UI/Star.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
StraighteningEnemyExist::StraighteningEnemyExist(StraighteningEnemyState* enemyState,
    const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_EnemyModel{},
	m_model{},
	m_position{},
    m_wall{wall},
    m_time{0.0f},
    m_isHit{true},
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale()),
	m_star{}
{

}

StraighteningEnemyExist::~StraighteningEnemyExist()
{
}

void StraighteningEnemyExist::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
   
	assert(resources);
	m_commonResources = resources;

	// �o�E���f�B���O�{�b�N�X
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);

	m_star = std::make_unique<Star>();
	m_star->Create(m_commonResources->GetDeviceResources());

}


void StraighteningEnemyExist::PreUpdate()
{
	m_position = m_enemy->GetPosition();
	m_star->SetOn(true);
}

void StraighteningEnemyExist::Update(const float& elapsedTime)
{
    m_time = elapsedTime;

	//m_star->SetPositionCircleRandom(m_position);
	//  �p�[�e�B�N���̍X�V
	m_star->Update(m_time,m_position);
	//�R�b�ȏ�~�܂����瓮����悤�ɂ���
	if (m_time > 3.0f)
	{
		m_enemy->ChangeState(m_enemy->GetStraighteningEnemySearch());
	}
}

void StraighteningEnemyExist::PostUpdate()
{
   
}

void StraighteningEnemyExist::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();

    // ���[���h�s����X�V����
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006f);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
    //�G�̕`��
    model.Draw(context, *states, world, view, projection);
	//  �p�[�e�B�N���̕`��
	m_star->Render(view, projection);
}

void StraighteningEnemyExist::Finalize()
{
}