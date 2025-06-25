/*
  @file EnemyTracking.cpp
  @brief �G�̒ǂ��������ԃN���X
*/
#include "pch.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Enemy/EnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Wall/Wall.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include "Game/Enemy/State/EnemySearch.h"
#include "Game/Player/PlayerState.h"


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemyTracking::EnemyTracking(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
    : m_enemy(enemyState), 
    m_wall(wall),
    m_currentGoalNo(0),
    m_position(DirectX::SimpleMath::Vector3::Zero),
    m_time(0.0f),
    m_player{player},
    m_commonResources{},
    m_exist{},
    m_scale{0.5f},
    m_modelScale{0.006f}
{
   
}

//---------------------------------------------------------
// �f�X�g���N�g
//---------------------------------------------------------
EnemyTracking::~EnemyTracking()
{
    
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void EnemyTracking::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
    m_commonResources = resources;
    m_position = pos;

    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    m_boundingBox.Center = m_position;
    m_boundingBox.Center.y += 0.5f;
    m_boundingBox.Extents = DirectX::SimpleMath::Vector3(m_scale);

    // SpriteBatch ������
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

    // �e�N�X�`���[�ǂݍ���
    DX::ThrowIfFailed(
        DirectX::CreateWICTextureFromFile(
            device,
            L"Resources/Textures/exclamationMark.png",
            nullptr,
            m_exclamationMarkTexture.ReleaseAndGetAddressOf()
        )
    );
   
}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void EnemyTracking::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_time = 0.0f;
    
}

//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void EnemyTracking::Update(const float& elapsedTime)
{
    m_time += elapsedTime;

    // �v���C���[�̈ʒu�Ɍ������Ĉړ����鏈��
    DirectX::SimpleMath::Vector3 targetPos = m_player->GetPosition();
    // �����␳
    targetPos.y += 0.5f;  

    DirectX::SimpleMath::Vector3 direction = targetPos - m_position;

    // �x�N�g���̒������擾
    float distance = direction.Length(); 
    // �����𐳋K������
    direction.Normalize();

    // �i�s�����Ɋ�Â���]���v�Z
    if (direction.LengthSquared() > 0.0f)
    {
        // Y���𒆐S�ɉ�]����ꍇ
        m_rotate = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
            atan2(direction.x, direction.z),  // Yaw (Y������̉�])
            0.0f,                               // Pitch
            0.0f                                // Roll
        );
    }

    if (distance > 0.1f) // ���̋����ȏ�ł���Έړ�
    {
        float speed = 2.0f;     // �G�̈ړ����x (�����\)
        m_position += direction * speed * elapsedTime; // �v���C���[�̕����Ɉړ�
    }

    for (auto& wall : m_wall)
    {
        m_position += CheckHitAndResolve(wall->GetBoundingBox(), m_boundingBox);
    }

    m_boundingBox.Center = m_position;
    m_enemy->SetBoundingBox(m_boundingBox);

    if (m_time > 5.0f)
    {
        m_enemy->ChangeState(m_enemy->GetEnemySearch());
    }

    this->PostUpdate();
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void EnemyTracking::PostUpdate()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void EnemyTracking::Render(ID3D11DeviceContext* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();
    // ���[���h�s����X�V����
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_modelScale);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotate);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
    // ���f����\������
    model.Draw(context, *states, world, view, projection);

    // === �G�́u����v1m�̍��W���܂��v�Z ===
    DirectX::SimpleMath::Vector3 headPosition = m_position + DirectX::SimpleMath::Vector3(0, 1.0f, 0);

    // === 3D �� 2D�X�N���[�����W�ϊ� ===
    DirectX::XMVECTOR screenPosVec = DirectX::XMVector3Project(
        DirectX::XMLoadFloat3(&headPosition),
        0.0f, 0.0f,
        static_cast<float>(m_commonResources->GetDeviceResources()->GetOutputSize().right),
        static_cast<float>(m_commonResources->GetDeviceResources()->GetOutputSize().bottom),
        0.0f, 1.0f,
        projection,
        view,
        DirectX::SimpleMath::Matrix::Identity
    );

    DirectX::SimpleMath::Vector3 screenPos2D;
    DirectX::XMStoreFloat3(&screenPos2D, screenPosVec);

    // === �X�v���C�g�`�� ===
    m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());

    float textureWidth = 726.0f;
    float textureHeight = 468.0f;
    float scale = 0.1f; // �����߂ɒ���

    DirectX::SimpleMath::Vector2 origin(textureWidth * 0.5f, textureHeight * 0.5f);
    DirectX::SimpleMath::Vector2 drawPos(screenPos2D.x, screenPos2D.y);

    // �e�N�X�`���`��FdrawPos���X�N���[�����W�̒��S�ʒu�ɂȂ�
    m_spriteBatch->Draw(
        m_exclamationMarkTexture.Get(),       // �e�N�X�`��
        drawPos,                      // �X�N���[���`��ʒu�i���S�j
        nullptr,                      // RECT
        DirectX::Colors::White,       // �F
        0.0f,                         // ��]�p
        origin,                       // �e�N�X�`���̌��_�i�����j
        scale,                        // �X�P�[�����O
        DirectX::SpriteEffects_None,  // �G�t�F�N�g
        0.0f                          // ���C���[�[�x
    );

    m_spriteBatch->End();
}

//---------------------------------------------------------
// ����
//---------------------------------------------------------
void EnemyTracking::Finalize()
{
}

//---------------------------------------------------------
// �ǂƂ̉����o������
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 EnemyTracking::CheckHitAndResolve(
    const DirectX::BoundingBox& wallBox, const DirectX::BoundingBox& entityBox)
{
    using namespace DirectX::SimpleMath;

    // �������Ă��Ȃ���Ή����o���Ȃ�
    if (!wallBox.Intersects(entityBox))   return Vector3::Zero;

    // �����o���x�N�g�����v�Z����
    Vector3 pushOut = Vector3::Zero;

    Vector3 wallCenter = wallBox.Center;
    Vector3 entityCenter = entityBox.Center;
    Vector3 wallExtents = wallBox.Extents;
    Vector3 entityExtents = entityBox.Extents;

    // �e�����Ƃ̉����o���ʂ��v�Z
    float deltaX = std::max(0.0f, (wallExtents.x + entityExtents.x) - abs(entityCenter.x - wallCenter.x));
    float deltaZ = std::max(0.0f, (wallExtents.z + entityExtents.z) - abs(entityCenter.z - wallCenter.z));

    // X������Z�����̂ǂ���̉����o�������������Ō���
    if (deltaX < deltaZ)
    {
        pushOut.x = (entityCenter.x > wallCenter.x) ? deltaX : -deltaX;
    }
    else
    {
        pushOut.z = (entityCenter.z > wallCenter.z) ? deltaZ : -deltaZ;
    }

    return pushOut;
}