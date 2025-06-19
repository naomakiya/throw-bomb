/*
  @file Player.cpp
  @brief �v���C���N���X
*/
#include "pch.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include"Game/Enemy/EnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/Resources.h"
#include"Game/Wall/Wall.h"
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <DirectXCollision.h>
#include <queue>
#include <random>
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/recastnavigation/Detour/Include/DetourNavMesh.h"
#include "Libraries/recastnavigation/Detour/Include/DetourNavMeshQuery.h"
#include"Libraries/MyLib/CollisionMesh.h"
#include "Libraries/MyLib/InputManager.h"
#include"Game/Enemy/State/EnemyTracking.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemyMovement::EnemyMovement(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_position{},
    m_wall{wall},
    m_scale(m_enemy->GetScale()),
    m_exist(m_enemy->GetExist()),
    m_isPaused(),
    m_pauseTimer{},
    m_pauseDuration{},
    m_timeUntilNextPause{},
    gen{},
    m_pauseChance(0.0f, 1.0f),
    m_isHit{false},
    m_collisionMesh{},
   /* m_navMesh{},
    m_navQuery{},*/
    m_patrolPath(patrolPath),
    m_currentGoalNo(0)
{
	//// �O���t�B�b�N�X���擾����
	//m_graphics = Graphics::GetInstance();
	//// �R���e�L�X�g���擾����
	//m_context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
}

EnemyMovement::~EnemyMovement()
{
}

void EnemyMovement::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    //auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ���f����ǂݍ��ޏ���
	//std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	//fx->SetDirectory(L"Resources/Models/Enemy");

	////// ���f�������[�h����
 //   m_enemyModel = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Enemy/Enemy.cmo", *fx);

	// �o�E���f�B���O�{�b�N�X
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);

    m_boundingSphere.Center = pos;
    m_boundingSphere.Radius = 2.5f;

    // ���b�V���𐶐�����
   /* m_collisionMesh = std::make_unique<mylib::CollisionMesh>();
    m_collisionMesh->Initialize(device, context, L"Floor");*/

    //m_enemyModel->UpdateEffects([&](DirectX::IEffect* effect)
    //    {
    //        DirectX::DX11::BasicEffect* basic = dynamic_cast<DirectX::DX11::BasicEffect*>(effect);
    //        if (basic != nullptr)
    //        {
    //            //basic->SetEmissiveColor(DirectX::Colors::White);
    //            basic->SetDiffuseColor(DirectX::Colors::Black);

    //        }
    //    });

    // �ŏ��̖ڕW�p�X��ݒ肷��
    SetNearestGoal();
}


void EnemyMovement::PreUpdate()
{
    m_position = m_enemy->GetPosition();
}

void EnemyMovement::Update(const float& elapsedTime,
    DirectX::SimpleMath::Vector3 target)
{
   
    // �G�����݂��Ȃ��ꍇ�͏������s��Ȃ�
    if (!m_exist) return;

    if (m_patrolPath.empty())
    {
        return;
    }

    // �ʒu���o�E���f�B���O�{�b�N�X�ɐݒ�
    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;

    // �ړ����x
    float moveSpeed = 1.5f;

    // ���݂̃S�[���Ɍ������Ĉړ�����
    DirectX::SimpleMath::Vector3 targetPosition = m_patrolPath[m_currentGoalNo];
    DirectX::SimpleMath::Vector3 direction = targetPosition - m_position;

    // �����𐳋K������
    direction.Normalize();

    // �i�s�����Ɋ�Â���]���v�Z
    if (direction.LengthSquared() > 0.0f)
    {
        m_rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
            atan2(direction.x, direction.z),  // Yaw (Y������̉�])
            0.0f,                             // Pitch
            0.0f                              // Roll
        );
    }

    // ���݂̈ʒu���X�V����
    m_position += direction * moveSpeed * elapsedTime;

    // �ڕW�ʒu�܂ł̋������v�Z����
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(m_position, targetPosition);

    // �S�[���ɓ��B������A���̃S�[���Ɉړ�����
    if (distanceToTarget < 0.5f)
    {
        m_currentGoalNo++;
        if (m_currentGoalNo >= m_patrolPath.size())
        {
            m_currentGoalNo = 0;
        }
    }

    // �ǂƂ̓����蔻��
    for (const auto& wall : m_wall)
    {
        CheckHit(wall->GetBoundingBox());
    }

    // �v���C���[�Ƃ̔���
    if (PlayerCheckHit(m_enemy->GetPlayer()->GetBoundingBox()))
    {
        m_enemy->ChangeState(m_enemy->GetEnemyTracking());
    }

    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
    m_enemy->SetBoundingSphere(m_boundingSphere);
}


void EnemyMovement::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

//�`��
void EnemyMovement::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();

    // ���[���h�s����X�V����
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006f);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

    // ���f����\������
    model.Draw(context, *states, world, view, projection);
 
}

void EnemyMovement::Finalize()
{
}

//---------------------------------------------------------
// �Փ˔��肷��
//---------------------------------------------------------
void EnemyMovement::CheckHit(DirectX::BoundingBox boundingBox)
{
    // �o�E���f�B���O�{�b�N�X���m�̏Փ˔�����Ƃ�
    m_isHit = m_boundingBox.Intersects(boundingBox);

    // �q�b�g���Ă��Ȃ���ΏI���
    if (!m_isHit) { return; }

    // �Փˎ��A�v���C���[�L�����N�^�[�������߂�����

    // AABB�p��min/max���v�Z����
    DirectX::SimpleMath::Vector3 aMin = DirectX::SimpleMath::Vector3(m_boundingBox.Center) - DirectX::SimpleMath::Vector3(m_boundingBox.Extents);
    DirectX::SimpleMath::Vector3 aMax = DirectX::SimpleMath::Vector3(m_boundingBox.Center) + DirectX::SimpleMath::Vector3(m_boundingBox.Extents);
    DirectX::SimpleMath::Vector3 bMin = DirectX::SimpleMath::Vector3(  boundingBox.Center) - DirectX::SimpleMath::Vector3(  boundingBox.Extents);
    DirectX::SimpleMath::Vector3 bMax = DirectX::SimpleMath::Vector3(  boundingBox.Center) + DirectX::SimpleMath::Vector3(  boundingBox.Extents);

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

    // �����߂��x�N�g��
    DirectX::SimpleMath::Vector3 pushBackVec = DirectX::SimpleMath::Vector3::Zero;

    // �߂荞�݂���ԏ��������������߂�
    if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz))
    {
        pushBackVec.x += dx;
    }
    else if (std::abs(dz) <= std::abs(dx) && std::abs(dz) <= std::abs(dy))
    {
        pushBackVec.z += dz;
    }
    else
    {
        pushBackVec.y += dy;
    }

    // �����߂�
    m_position += pushBackVec;
    m_boundingBox.Center = m_boundingBox.Center + pushBackVec;
}

void EnemyMovement::SetNearestGoal()
{
    if (m_patrolPath.empty())
    {
        m_currentGoalNo = 0;
        return;
    }

    // ���݈ʒu�Ɗe�p�X�ʒu�̋������v�Z���A�ł��߂��ڕW��������
    float minDistanceSq = FLT_MAX;
    int nearestGoal = 0;

    for (size_t i = 0; i < m_patrolPath.size(); ++i)
    {
        float distanceSq = DirectX::SimpleMath::Vector3::DistanceSquared(m_position, m_patrolPath[i]);
        if (distanceSq < minDistanceSq)
        {
            minDistanceSq = distanceSq;
            nearestGoal = static_cast<int>(i);
        }
    }

    m_currentGoalNo = nearestGoal;
}

bool EnemyMovement::PlayerCheckHit(DirectX::BoundingBox boundingBox)
{
    // AABB�̍ł��߂��_���v�Z����
    DirectX::SimpleMath::Vector3 closestPoint = m_boundingSphere.Center;

    // �e���ɂ��āA�X�t�B�A�̒��S��AABB�̊O���ɂ���ꍇ�A���̎���AABB�̒[�_���g�p����
    if (m_boundingSphere.Center.x < boundingBox.Center.x - boundingBox.Extents.x)
        closestPoint.x = boundingBox.Center.x - boundingBox.Extents.x;
    else if (m_boundingSphere.Center.x > boundingBox.Center.x + boundingBox.Extents.x)
        closestPoint.x = boundingBox.Center.x + boundingBox.Extents.x;

    if (m_boundingSphere.Center.y < boundingBox.Center.y - boundingBox.Extents.y)
        closestPoint.y = boundingBox.Center.y - boundingBox.Extents.y;
    else if (m_boundingSphere.Center.y > boundingBox.Center.y + boundingBox.Extents.y)
        closestPoint.y = boundingBox.Center.y + boundingBox.Extents.y;

    if (m_boundingSphere.Center.z < boundingBox.Center.z - boundingBox.Extents.z)
        closestPoint.z = boundingBox.Center.z - boundingBox.Extents.z;
    else if (m_boundingSphere.Center.z > boundingBox.Center.z + boundingBox.Extents.z)
        closestPoint.z = boundingBox.Center.z + boundingBox.Extents.z;

    // �ł��߂��_�ƃX�t�B�A�̒��S�Ƃ̋������v�Z����
    DirectX::SimpleMath::Vector3 distanceVec = closestPoint - m_boundingSphere.Center;
    float distanceSq = distanceVec.LengthSquared();

    // �����̓�悪�X�t�B�A�̔��a�̓��ȉ��ł���΁A�Փ˂��������Ă���
    return distanceSq <= m_boundingSphere.Radius * m_boundingSphere.Radius;
}
