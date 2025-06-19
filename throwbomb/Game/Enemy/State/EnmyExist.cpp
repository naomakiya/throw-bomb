/*
  @file  EnemyExist.cpp
  @brief �G�̐�����ԃN���X
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/Resources.h"
#include"Game/Enemy/EnemyState.h"
#include"Game/Enemy/State/EnmyExist.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Wall/Wall.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemyExist::EnemyExist(EnemyState* enemyState, 
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
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale())
{

}

//---------------------------------------------------------
// �f�X�g���N�g
//---------------------------------------------------------
EnemyExist::~EnemyExist()
{
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void EnemyExist::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	// �o�E���f�B���O�{�b�N�X
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);

}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void EnemyExist::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_time = 0.0f;
}

//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void EnemyExist::Update(const float& elapsedTime)
{
    //���Ԍo��
    m_time += elapsedTime;
    //�R�b�o�������Ԃ̕ύX
    if (m_exist)
    {
        m_enemy->ChangeState(m_enemy->GetEnemyMovement());
    }
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void EnemyExist::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void EnemyExist::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{

    auto states = m_commonResources->GetCommonStates();
    // ���[���h�s����X�V����
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006f);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
    //���f���̕`��
    model.Draw(context, *states, world, view, projection);

}

//---------------------------------------------------------
// ����
//---------------------------------------------------------
void EnemyExist::Finalize()
{
}