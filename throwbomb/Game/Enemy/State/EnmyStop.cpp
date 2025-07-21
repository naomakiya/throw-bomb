/*
  @file  EnemyStop.cpp
  @brief �G�̎~�܂�N���X
*/
#include "pch.h"
#include "Framework/DeviceResources.h"
#include "Framework/CommonResources.h"
#include "Game/Enemy/State/EnmyStop.h"
#include "Game/Enemy/State/EnmyExist.h"
#include "Game/Enemy/EnemyState.h"
#include "Game/UI/Star.h"


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
 EnemyStop:: EnemyStop(EnemyState* enemyState, 
    const std::vector<std::unique_ptr<Wall>>& wall)
	:
	 m_commonResources{ nullptr },
	 m_enemy{ enemyState },
	 m_wall{ wall },
	 m_star{ nullptr },
	 m_boundingBox{},
	 m_position{ m_enemy->GetPosition() },
	 m_stoptime{0},
     m_isExist( m_enemy->GetExist() )
	


{

}

//---------------------------------------------------------
// �f�X�g���N�g
//---------------------------------------------------------
 EnemyStop::~ EnemyStop()
{
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void  EnemyStop::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	// �������̐��̐���
	m_star = std::make_unique<Star>();
	m_star->Create(m_commonResources->GetDeviceResources());
}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void  EnemyStop::PreUpdate()
{	
	// �~�߂鎞�Ԃ̏�����
	m_stoptime = 0.0f;
	// ���݂̈ʒu���̎擾
	m_position = m_enemy->GetPosition();
	// ���݂̃o�E�f�B���O�a�n�w�̎擾
	m_boundingBox = m_enemy->GetBoundingBox();
	// ����z�u����
	m_star->SetOn(true);
}

//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void  EnemyStop::Update(const float& elapsedTime)
{
	// ���Ԍo�߂��h����
	m_stoptime += elapsedTime;
	//  ���p�[�e�B�N���̍X�V
	m_star->Update(elapsedTime,m_position);
   //2�b�ȏ�~�܂����瓮����悤�ɂ���
	if (m_stoptime > 2.0f){
		// ������Ԃɕς���
		m_enemy->ChangeState(m_enemy->GetEnemyExist());
	}
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void  EnemyStop::PostUpdate()
{
	// ���݂̈ʒu����ݒ肷��
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void  EnemyStop::Render(ID3D11DeviceContext* context, DirectX::CommonStates* states,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
	// ���[���h�s����X�V����
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(EnemyState::ENEMYMODELSCALE);
	world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
	// ���f����\������
	model.Draw(context, *states, world, view, projection);
	//  �p�[�e�B�N���̕`��
	m_star->Render(view, projection);
}

//---------------------------------------------------------
// ����
//---------------------------------------------------------
void  EnemyStop::Finalize()
{
}