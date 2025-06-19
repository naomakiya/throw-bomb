/*
  @file  EnemyPointMovement.cpp
  @brief �G���ړI�̏ꏊ�Ɉړ���ԃN���X
*/
#include "pch.h"
#include "Game/Enemy/State/EnemyPointMovement.h"
#include "Game/Enemy/EnemyState.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Framework/Resources.h"
#include "Game/Wall/Wall.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Game/Enemy/State/EnemyTracking.h"
#include "Game/Enemy/State/EnemySearch.h"
#include "Game/Enemy/State/EnmyStop.h"
#include "Game/Player/PlayerState.h"
#include "Game/Collision/Collision.h"
#include "Game/Bom/BomState.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemyPointMovement::EnemyPointMovement(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall)
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
    m_modelScale(0.006f)
{
	
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
EnemyPointMovement::~EnemyPointMovement()
{
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void EnemyPointMovement::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
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
void EnemyPointMovement::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_patrolPathPos = m_enemy->GetPointPosition();
}
 
//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void EnemyPointMovement::Update(const float& elapsedTime)
{
   
    // �G�����݂��Ȃ��ꍇ�͏������s��Ȃ�
    if (!m_exist ) return;

    // �o�E���f�B���O�̍X�V
    UpdateBounding();
    // �p�g���[������
    UpdateDirectionAndRotation(elapsedTime);
   //�ǂ̓����蔻��
    CheckWallCollision();
    // �G�̓���
    SyncWithEnemyState();
   
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void EnemyPointMovement::PostUpdate()
{
    
    m_enemy->SetPosition(m_position);
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void EnemyPointMovement::Render(ID3D11DeviceContext* context,
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
void EnemyPointMovement::Finalize()
{
}

void EnemyPointMovement::HPDown()
{
    m_enemy->SetExist(false);
}

void EnemyPointMovement::UpdateBounding()
{
    // �ʒu���o�E���f�B���O�{�b�N�X�E�X�t�B�A�̐ݒ�
    m_boundingBox.Center = m_position;
    m_boundingSphere.Center = m_position;
}

void EnemyPointMovement::UpdateDirectionAndRotation(float elapsedTime)
{
    // ���݈ʒu�ƖړI�n�̊Ԃ��ړ����郍�W�b�N
    

    // �v���C���[�̈ʒu�Ɍ������Ĉړ����鏈��
    DirectX::SimpleMath::Vector3 targetPos = m_patrolPathPos;
    // �����␳
    targetPos.y += 1.0f;

    DirectX::SimpleMath::Vector3 direction = targetPos - m_position;
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
    float speed = 2.0f;
    m_position += direction * speed * elapsedTime;

    m_enemy->SetPosition(m_position);

    // ���B�`�F�b�N�i��j
    if ((m_patrolPathPos - m_position).Length() < 1.5f) {
        // �ړI�n�ɓ���������X�g�b�v�X�e�[�g�ɑJ�ڂ���Ȃ�
        m_enemy->ChangeState(m_enemy->GetEnemySearch());
    }

}

void EnemyPointMovement::CheckWallCollision()
{
    // �ǂƂ̓����蔻��
    for (const auto& wall : m_wall)
    {
        mylib::Collision::CheckHit(m_boundingBox, wall->GetBoundingBox());
    }
}

void EnemyPointMovement::SyncWithEnemyState()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
    m_enemy->SetBoundingSphere(m_boundingSphere);
}
