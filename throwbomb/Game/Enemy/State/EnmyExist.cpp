/*
  @file  EnemyExist.cpp
  @brief �G�̐�����ԃN���X
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Enemy/EnemyState.h"
#include"Game/Enemy/State/EnmyExist.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Wall/Wall.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemyExist::EnemyExist(EnemyState* enemyState)
	:
    m_commonResources{ nullptr },
	m_enemy{ enemyState },
	m_position{ m_enemy->GetPosition() },
    m_time{ 0.0f },
    m_isExist(m_enemy->GetExist())
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
void EnemyExist::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void EnemyExist::PreUpdate()
{
    // �ʒu���̍X�V
    m_position = m_enemy->GetPosition();
    // �o�E�f�B���OBOX�̍X�V
    m_boundingBox = m_enemy->GetBoundingBox();
    //���Ԃ̏�����
    m_time = 0.0f;
}

//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void EnemyExist::Update(const float& elapsedTime)
{
    m_time += elapsedTime;
    // ����Ȃ��ԕύX(�����̊Ԃ��J���Ă���ړ�)
    if (m_isExist && m_time > 0.25f){
        // ������Ԃ�
        m_enemy->ChangeState(m_enemy->GetEnemyMovement());
    }
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void EnemyExist::PostUpdate()
{
    // �ʒu�̐ݒ�
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void EnemyExist::Render(ID3D11DeviceContext* context, DirectX::CommonStates* states,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    // ���[���h�s����X�V����
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(EnemyState::ENEMYMODELSCALE);
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