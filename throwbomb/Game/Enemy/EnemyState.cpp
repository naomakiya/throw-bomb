/*
  @file  EnemyState.cpp
  @brief �G�̏�ԃN���X
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Enemy/EnemyState.h"
#include "Game/Enemy/State/EnmyExist.h"
#include "Game/Enemy/State/EnemyMovement.h"
#include "Game/Enemy/State/EnemyTracking.h"
#include "Game//Enemy/State/EnmyStop.h"
#include "Game/Enemy/State/EnemyPointMovement.h"
#include "Game/Enemy/State//EnemySearch.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomStay.h"
#include "Game/Shadow/Shadow.h"

void EnemyState::HPDown()
{
    for (int i = 0; i < 3; i++)
    {
        switch (m_player->GetBomState(i)->GetBomPresent())
        {
        case IBomState::MOVEMENT:

            break;
        case IBomState::EXPLOSION:
            m_hart -= 5;
            break;
        default:
            break;
        }
    }

    //�̗͂��Ȃ��Ȃ琶����false�ɂ���
    if (m_hart <= 0) isExist = false;
}

void EnemyState::SetPointPosition(DirectX::SimpleMath::Vector3& position)
{ 
    m_pointPosition = position;
    ChangeState(m_pointMovement.get());
    
}

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemyState::EnemyState( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath, const DirectX::SimpleMath::Vector3& position)
    :
    m_position{ position },
    m_enemyModel{},
    m_commonResources{},
    m_angle{ 0 },
    m_scale{ 0.5f },
    m_currentState{},
    m_player{player},
    m_totalSeconds{},
    m_wall{wall},
    isExist{true},
    m_hart{5},
    m_patrolPath{ patrolPath },
    m_boudingSphere{},
    m_collisionDebugRenderer{},
    m_shadow{},
    m_stop{},
    m_pointMovement{}
{
   
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
EnemyState::~EnemyState()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void EnemyState::Initialize(CommonResources* resources)
{
    assert(resources);
    m_commonResources = resources;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
    //���S�̈ړ�
    m_position.y += 0.5f;

    m_boudingSphere.Center = m_position;
    m_boudingSphere.Radius = 0.25f;

    // �e���쐬����
    m_shadow = std::make_unique<Shadow>();
    m_shadow->Initialize(device, context);

    m_exist = std::make_unique<EnemyExist>(this);
    m_exist->Initialize(m_commonResources);

    m_movement = std::make_unique<EnemyMovement>(this, m_wall, m_patrolPath);
    m_movement->Initialize(m_commonResources);

    m_pointMovement = std::make_unique<EnemyPointMovement>(this, m_wall);
    m_pointMovement->Initialize(m_commonResources);

    m_tracking = std::make_unique<EnemyTracking>(this, m_wall,m_player);
    m_tracking->Initialize(m_commonResources);

    m_stop = std::make_unique<EnemyStop>(this, m_wall);
    m_stop->Initialize(m_commonResources);

    m_search = std::make_unique<EnemySearch>(this, m_wall, m_player);
    m_search->Initialize(m_commonResources);
    
    m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device,context);

    m_currentState = m_exist.get();
}

//---------------------------------------------------------
// ��Ԃ�ς���
//---------------------------------------------------------
void EnemyState::ChangeState(IEnemyState* newState)
{
    //// �V�K�̏�ԑJ�ڑO�Ɏ���X�V���s��
    m_currentState->PostUpdate();
    // �V�K�̏�Ԃ����݂̏�Ԃɐݒ肷��
    m_currentState = newState;
    //// �V�K�̏�ԑJ�ڌ�Ɏ��O�X�V���s��
    m_currentState->PreUpdate();
}

//---------------------------------------------------------
// ���O�X�V����
//---------------------------------------------------------
void EnemyState::PreUpdate()
{
    
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void EnemyState::Update(const float& elapsedTime)
{
    // ���݂̏�Ԃ��X�V����
    m_currentState->Update(elapsedTime);
   //���e�̐���
    for (int i = 0; i < 3; i++)
    {   
        //�G��ĂȂ��Ȃ�X�L�b�v
        if (!m_boundingBox.Intersects(m_player->GetBomState(i)->GetBoundingSphere())) continue;

        //���e���ړ����Ă���Ȃ�G���~�߂�
        if (m_player->GetBomState(i)->GetBomPresent() == BomState::MOVEMENT)
        {
            ChangeState(m_stop.get());
            m_player->GetBomState(i)->SetVelocity(DirectX::SimpleMath::Vector3(0, 0, 0));
            m_player->GetBomState(i)->ChangeState(m_player->GetBomState(i)->GetBomStay());
        }
    }
    
}

//---------------------------------------------------------
// ����X�V����
//---------------------------------------------------------
void EnemyState::PostUpdate()
{

}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void EnemyState::Render(ID3D11DeviceContext* context,
    DirectX::CommonStates* states,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
    // ���݂̏�Ԃ�`�悷��
    m_currentState->Render(context,states,view, projection,model);
    // ���@�̉e��`�悷��
    m_shadow->Render(context, states, view, projection, m_position);

#ifdef _DEBUG
    //�f�o�b�N�̃J���[���̍쐬
    const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
        DirectX::Colors::Red.f[1],
        DirectX::Colors::Red.f[2],
        DirectX::Colors::Red.f[3]);

    // �f�o�b�O�`��
    m_collisionDebugRenderer->RenderBoundingBox(m_boundingBox, view, projection, color);
#endif
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void EnemyState::Finalize()
{
   
}


