/*
  @file  EnemyMovement.cpp
  @brief �G�̈ړ���ԃN���X
*/
#include "pch.h"
#include "Game/Enemy/State/EnemyMovement.h"
#include "Game/Enemy/EnemyState.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Wall/Wall.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Game/Enemy/State/EnemyTracking.h"
#include "Game/Player/PlayerState.h"
#include "Game/Collision/Collision.h"
#include "Game/Bom/BomState.h"

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
    m_moveSpeed(2.5f),
    m_isHit{false},
    m_patrolPath(patrolPath),
    m_currentGoalNo(0),
    m_modelScale(0.006f),
    goalArrivalThreshold{0.5f}
{
	
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
EnemyMovement::~EnemyMovement()
{
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void EnemyMovement::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	// �o�E���f�B���O�{�b�N�X
    m_position = pos;
	m_boundingBox.Center = pos;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(0.5f);

    m_boundingSphere.Center = pos;
    m_boundingSphere.Radius = 2.5f;

    // �ŏ��̖ڕW�p�X��ݒ肷��
    SetNearestGoal();
}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void EnemyMovement::PreUpdate()
{
    m_position = m_enemy->GetPosition();
}
 
//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void EnemyMovement::Update(const float& elapsedTime)
{
   
    // �G�����݂��Ȃ��ꍇ�͏������s��Ȃ� �܂��́@�p�X�����݂��Ȃ��ꍇ�������Ȃ�
    if (!m_exist || m_patrolPath.empty()) return;

    // �o�E���f�B���O�̍X�V
    UpdateBounding();
    // �p�g���[������
    UpdateDirectionAndRotation(elapsedTime);
   //�ǂ̓����蔻��
    CheckWallCollision();
    // �{���̉���s��
    CheckAvoidanceFromBombs();
    //�v���C���[�̔���
    CheckCollisionWithPlayer();
    // �G�̓���
    SyncWithEnemyState();
   
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void EnemyMovement::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void EnemyMovement::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();

    // ���[���h�s����X�V����
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_modelScale);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotate);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

    // ���f����\������
    model.Draw(context, *states, world, view, projection);
 
}

//---------------------------------------------------------
// �㏈��
//---------------------------------------------------------
void EnemyMovement::Finalize()
{
}

//---------------------------------------------------------
// �������ꏊ�̐ݒ�
//---------------------------------------------------------
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

void EnemyMovement::HPDown()
{
    m_enemy->SetExist(false);
}
//---------------------------------------------------------
// �ł������S�[��
//---------------------------------------------------------
int EnemyMovement::GetFurthestGoalFrom()
{
    const auto& playerPos = m_enemy->GetPlayer()->GetPosition();
    float maxDistanceSq = 0.0f;
    int furthestIndex = m_currentGoalNo;

    for (size_t i = 0; i < m_patrolPath.size(); ++i)
    {
        float distSq = DirectX::SimpleMath::Vector3::DistanceSquared(playerPos, m_patrolPath[i]);
        if (distSq > maxDistanceSq)
        {
            maxDistanceSq = distSq;
            furthestIndex = static_cast<int>(i);
        }
    }

    return furthestIndex;
}

void EnemyMovement::UpdateBounding()
{
    // �ʒu���o�E���f�B���O�{�b�N�X�E�X�t�B�A�̐ݒ�
    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;
}

void EnemyMovement::UpdateDirectionAndRotation(float elapsedTime)
{
    // ���݂̃S�[���Ɍ������Ĉړ�����
    DirectX::SimpleMath::Vector3 targetPosition = m_patrolPath[m_currentGoalNo];
    DirectX::SimpleMath::Vector3 direction = targetPosition - m_position;

    // �����𐳋K������
    direction.Normalize();

    // �i�s�����Ɋ�Â���]���v�Z
    if (direction.LengthSquared() > 0.0f)
    {
        m_rotate = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
            atan2(direction.x, direction.z),  // Yaw (Y������̉�])
            0.0f,                             // Pitch
            0.0f                              // Roll
        );
    }

    // ���݂̈ʒu���X�V����
    m_position += direction * m_moveSpeed * elapsedTime;

    // �ڕW�ʒu�܂ł̋������v�Z����
    float distanceToTarget = DirectX::SimpleMath::Vector3::Distance(m_position, targetPosition);

    // �S�[���ɓ��B������A���̃S�[���Ɉړ�����
    if (distanceToTarget < goalArrivalThreshold)
    {
        m_currentGoalNo++;
        if (m_currentGoalNo >= m_patrolPath.size())
        {
            m_currentGoalNo = 0;
        }
    }
}

void EnemyMovement::CheckWallCollision()
{
    // �ǂƂ̓����蔻��
    for (const auto& wall : m_wall)
    {
        mylib::Collision::CheckHit(m_boundingBox, wall->GetBoundingBox());
    }
}

void EnemyMovement::CheckAvoidanceFromBombs()
{
    // ��������F�{�����߂�
    float avoidRadius = 3.5f;
    for (int i = 0; i < 3; i++)
    {
        float playerDistance = DirectX::SimpleMath::Vector3::Distance(m_position, m_enemy->GetPlayer()->GetBomState(i)->GetPosition());

        if (playerDistance < avoidRadius)
        {
            // �v���C���[����ł������p�g���[���n�_�ɓ�����
            m_currentGoalNo = GetFurthestGoalFrom();
        }
    }
}

void EnemyMovement::CheckCollisionWithPlayer()
{
    // �v���C���[�Ƃ̔���
    if (mylib::Collision::BoundingCheckCollision(m_enemy->GetPlayer()->GetBoundingBox(), m_boundingSphere))
    {
        m_enemy->ChangeState(m_enemy->GetEnemyTracking());
    }
}

void EnemyMovement::SyncWithEnemyState()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
    m_enemy->SetBoundingSphere(m_boundingSphere);
}
