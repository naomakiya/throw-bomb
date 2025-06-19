/*
  @file  EnemyStop.cpp
  @brief �G�̎~�܂�N���X
*/
#include "pch.h"
#include "Framework/Resources.h"
#include "Game/Enemy/State/EnmyStop.h"
#include "Game/Enemy/State/EnmyExist.h"
#include "Game/Enemy/EnemyState.h"
#include "Framework/CommonResources.h"
#include "Game/UI/Star.h"


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
 EnemyStop:: EnemyStop(EnemyState* enemyState, 
    const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_EnemyModel{},
	m_model{},
	m_position{},
    m_wall{wall},
	 m_stoptime{0},
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale()),
	 m_star{}
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
void  EnemyStop::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	// �o�E���f�B���O�{�b�N�X
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);


	// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/Enemy");

	//// ���f�������[�h����
	m_enemyModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Enemy/Enemy.cmo", *fx);


	m_star = std::make_unique<Star>();
	m_star->Create(m_commonResources->GetDeviceResources());
}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void  EnemyStop::PreUpdate()
{
	m_stoptime = 0.0f;
	m_position = m_enemy->GetPosition();
	m_star->SetOn(true);
}

//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void  EnemyStop::Update(const float& elapsedTime)
{
	m_stoptime += elapsedTime;

	//  �p�[�e�B�N���̍X�V
	m_star->Update(elapsedTime,m_position);
   //�R�b�ȏ�~�܂����瓮����悤�ɂ���
	if (m_stoptime > 2.5f)
	{
		m_enemy->ChangeState(m_enemy->GetEnemyExist());
	}
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void  EnemyStop::PostUpdate()
{
    m_enemy->SetPosition(m_position);

	
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void  EnemyStop::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
	auto states = m_commonResources->GetCommonStates();

	// ���[���h�s����X�V����
	DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006);
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