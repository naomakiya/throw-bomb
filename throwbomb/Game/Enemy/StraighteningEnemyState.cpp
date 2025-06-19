#include"pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Enemy/StraighteningEnemyState.h"
#include "Game/Enemy/StraighteningEnemyState/StraighteningEnemySearch.h"
#include "Game/Enemy/StraighteningEnemyState/StraighteningEnemyDash.h"
#include "Game/Enemy/StraighteningEnemyState/StraighteningEnemyExist.h"
#include "Game/Scene/PlayScene.h"
#include "Game/Wall/Wall.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Game/Bom/BomState.h"
#include "Recast.h"
#include "Game/Shadow/Shadow.h"
#include "Game/Collision/Collision.h"
#include "Game/Bom/State/BomStay.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

void StraighteningEnemyState::HPDown()
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
}

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
StraighteningEnemyState::StraighteningEnemyState( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
    :
    m_position{},
    m_enemyModel{},
    m_commonResources{},
    m_mass{ 0 },
    m_angle{ 0 },
    m_scale{ 0.5f },
    m_currentState{},
    m_player{player},
    m_wall{wall},
    isExist{true},
    m_hart{5},
    m_boudingSphere{},
    m_collisionDebugRenderer{},
    m_shadow{},
    m_search{},
    m_shadowPosition{}
{

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
StraighteningEnemyState::~StraighteningEnemyState()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void StraighteningEnemyState::Initialize(CommonResources* resources,DirectX::SimpleMath::Vector3 pos)
{
    assert(resources);
    m_commonResources = resources;
    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
   

    m_position = pos;
    m_shadowPosition = pos;

    // �e���쐬����
    m_shadow = std::make_unique<Shadow>();
    m_shadow->Initialize(device, context);

    m_exist = std::make_unique<StraighteningEnemyExist>(this,m_wall);
    m_exist->Initialize(m_commonResources,pos);

    m_search = std::make_unique<StraighteningEnemySearch>(this, m_wall, m_player);
    m_search->Initialize(m_commonResources,pos);

    m_dash = std::make_unique<StraighteningEnemyDash>(this, m_wall,m_player);
    m_dash->Initialize(m_commonResources, pos);

    /* m_stop = std::make_unique<EnemyStop>(this, m_wall);
    m_stop->Initialize(m_commonResources, pos);*/

    m_boudingSphere.Center = m_position;
    m_boudingSphere.Radius = 0.5;

    m_boundingBox.Center = m_position;
    m_boundingBox.Extents = Vector3(0.5f, 0.5f, 0.5f) ;

    m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device,context);

    m_currentState = m_search.get();
}

void StraighteningEnemyState::ChangeState(IEnemyState* newState)
{
    //// �V�K�̏�ԑJ�ڑO�Ɏ���X�V���s��
    m_currentState->PostUpdate();
    // �V�K�̏�Ԃ����݂̏�Ԃɐݒ肷��
    m_currentState = newState;
    //// �V�K�̏�ԑJ�ڌ�Ɏ��O�X�V���s��
    m_currentState->PreUpdate();
}

void StraighteningEnemyState::PreUpdate()
{
}


//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void StraighteningEnemyState::Update(const float& elapsedTime)
{
    m_position.y -= PlayerState::GRVITY * elapsedTime;
    // ���݂̏�Ԃ��X�V����
    m_currentState->Update(elapsedTime);
    m_boundingBox.Center = m_position;
    //�����߂��l
    DirectX::SimpleMath::Vector3 m_pushBack;
    for (auto& wall : m_wall)
    {
        m_pushBack = CheckHitAndResolve(wall->GetBoundingBox());

        // �����߂������������ꍇ�̂݃|�W�V�������X�V
        if (std::abs(m_pushBack.Length()) > 0.00f || std::abs(m_pushBack.Length()) < 0.00f)
        {
            m_position += m_pushBack;
        }
    }
    if (m_position.y < 1.00f)
    {
        m_position.y = 1.00f;
    }
    if (m_hart <= 0)
    {
        isExist = false;
    }

    for (int i = 0; i < 3; i++)
    {
        //�G��ĂȂ��Ȃ�X�L�b�v
        if (!m_boundingBox.Intersects(m_player->GetBomState(i)->GetBoundingSphere())) continue;

        if (m_player->GetBomState(i)->GetBomPresent() == BomState::MOVEMENT)
        {
            ChangeState(m_exist.get());
            m_player->GetBomState(i)->SetVelocity(Vector3(0, 0, 0));
            m_player->GetBomState(i)->ChangeState(m_player->GetBomState(i)->GetBomStay());
        }
    }
}

void StraighteningEnemyState::PostUpdate()
{

}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void StraighteningEnemyState::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();

    //�e�`��
    m_currentState->Render(context,view, projection,model);

    //�e�̈ʒu����
    m_shadowPosition = m_position;
    m_shadowPosition.y -= 0.45f;
    m_shadowPosition.z += 0.20f;
    // ���@�̉e��`�悷��
    m_shadow->Render(context, states, view, projection, m_shadowPosition);
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void StraighteningEnemyState::Finalize()
{
    // ���������Z�b�g����
    m_exist.reset();
}

//---------------------------------------------------------
// �ǂƂ̉����o������
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 StraighteningEnemyState::CheckHitAndResolve(
    const DirectX::BoundingBox& wallBox)
{

    using namespace DirectX::SimpleMath;
    // �o�E���f�B���O�{�b�N�X���m�̏Փ˔�����Ƃ� || �����Ă���Ȃ�A�Փ˂Ȃ�
    if (!m_boundingBox.Intersects(wallBox)) { return Vector3::Zero; }
    // AABB�p��min/max���v�Z����
    Vector3 aMin = m_boundingBox.Center - m_boundingBox.Extents;
    Vector3 aMax = m_boundingBox.Center + m_boundingBox.Extents;
    Vector3 bMin = wallBox.Center - wallBox.Extents;
    Vector3 bMax = wallBox.Center + wallBox.Extents;

    // �e���̍������v�Z����
    float dx1 = bMax.x - aMin.x;
    float dx2 = bMin.x - aMax.x;
    float dy1 = bMax.y - aMin.y;
    float dy2 = bMin.y - aMax.y;
    float dz1 = bMax.z - aMin.z;
    float dz2 = bMin.z - aMax.z;

    // �e���ɂ��āA��Βl�̏������������̂߂荞�ݗʂƂ���FAABB�̏d�Ȃ�����������肷��
    float dx = std::abs(dx1) < std::abs(dx2) ? dx1 : dx2;
    float dy = std::abs(dy1) < std::abs(dy2) ? dy1 : dy2;
    float dz = std::abs(dz1) < std::abs(dz2) ? dz1 : dz2;

    // �Փˎ��̉����߂��x�N�g��
    Vector3 pushBackVec = Vector3::Zero;

    // �Փ�臒l�𒴂����ꍇ�̂݉����߂�
    if (std::abs(dx) > 0.0f || std::abs(dy) > 0.0f || std::abs(dz) > 0.0f)
    {
        // �܂�X�������ł̏���
        if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz))
        {
            pushBackVec.x = dx; // X�������ŉ����߂�
        }
        // ����Z�������ł̏���
        else if (std::abs(dz) <= std::abs(dy))
        {
            pushBackVec.z = dz; // Z�������ŉ����߂�
        }
    }

    pushBackVec.y = dy;

    if (dy < 0.0f)
    {
        pushBackVec.y = 0.0f;
    }

    // �ʒu�������݂ɐU������h�����߁A�����ȏC���ʂ𖳎�
    if (std::abs(pushBackVec.x) < 0.10f) { pushBackVec.x = 0.0f; }
    ///*if (std::abs(pushBackVec.y) < 0.10f) { pushBackVec.y = 0.0f; }*/
    if (std::abs(pushBackVec.z) < 0.10f) { pushBackVec.z = 0.0f; }

    // �߂荞�݂̍��̉����Ԃ�
    return pushBackVec;
}
