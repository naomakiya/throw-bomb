/*
  @file EnemySearch.cpp
  @brief �T���Ă����ԃN���X
*/
#include "pch.h"
#include"Game/Enemy/State/EnemySearch.h"
#include"Game/Enemy/EnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Wall/Wall.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Enemy/State/EnemyTracking.h"


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemySearch::EnemySearch(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
    : m_enemy(enemyState), 
    m_wall(wall),
    m_currentGoalNo(0),
    m_position(DirectX::SimpleMath::Vector3::Zero),
    m_time(0.0f),
    m_player{player},
    m_commonResources{},
    m_exist{},
    m_scale{0.5f},
    m_modelScale{0.006f},
    m_rotate{},
    m_textureAngle{ 0.0f },
    m_totalTime{0.0f}
{
   
}

//---------------------------------------------------------
// �f�X�g���N�g
//---------------------------------------------------------
EnemySearch::~EnemySearch()
{
    
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void EnemySearch::Initialize(CommonResources* resources)
{
    m_commonResources = resources;

    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    // SpriteBatch ������    
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

    // �e�N�X�`���[�ǂݍ���
    DX::ThrowIfFailed(
        DirectX::CreateWICTextureFromFile(
            device,
            L"Resources/Textures/mystery.png", 
            nullptr,
            m_mysteryTexture.ReleaseAndGetAddressOf()
        )
    );
    // �o�E���f�B���O�{�b�N�X�̃T�C�Y��ݒ�i��2.0�A����2.0�A���s��2.0�̗����́j
    m_searchBoundingBox = DirectX::BoundingOrientedBox(m_position, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.5f), DirectX::SimpleMath::Quaternion::Identity);
}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void EnemySearch::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_time = 0.0f;
    m_textureAngle = 0.0f;
    m_totalTime = 0.0f;
    m_boundingBox = m_enemy->GetBoundingBox();
    m_searchBoundingBox.Center = m_enemy->GetPosition();
}

//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void EnemySearch::Update(const float& elapsedTime)
{
    m_time += elapsedTime;
    m_textureAngle += sin(elapsedTime);
   
    float angleRad = sinf(DirectX::XM_PI * m_time) * (DirectX::XM_PI / 4.0f);
    
    m_textureAngle = angleRad;
    // 1�b��30�x��]����
    float rotationSpeed = DirectX::XMConvertToRadians(45.0f) * elapsedTime;

    // Y����]�̃N�H�[�^�j�I�����쐬���A���݂̉�]�ɓK�p
    DirectX::SimpleMath::Quaternion rotationDelta = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, rotationSpeed);
    m_rotate = m_rotate * rotationDelta;
    m_rotate.Normalize(); // ���K��

    // �G�̑O���x�N�g�����擾
    DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Forward;
    forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotate));

    // �T���p�̔��̍X�V

    // �O���ɔz�u
    m_searchBoundingBox.Center = m_position + forward * -1.5f;
    // ��]��K�p
    m_searchBoundingBox.Orientation = m_rotate;

    for (auto& wall : m_wall)
    {
        m_position += CheckHitAndResolve(wall->GetBoundingBox(), m_boundingBox);
    }

    if (m_searchBoundingBox.Intersects(m_player->GetBoundingBox()))
    {
        m_enemy->ChangeState(m_enemy->GetEnemyTracking());
    }

    if (m_time > 5.0f)
    {
        m_enemy->ChangeState(m_enemy->GetEnemyMovement());
    }

    this->PostUpdate();
}

//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void EnemySearch::PostUpdate()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void EnemySearch::Render(ID3D11DeviceContext* context, DirectX::CommonStates* states, 
    const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
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
        m_mysteryTexture.Get(),       // �e�N�X�`��
        drawPos,                      // �X�N���[���`��ʒu�i���S�j
        nullptr,                      // RECT
        DirectX::Colors::White,       // �F
        m_textureAngle,                         // ��]�p
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
void EnemySearch::Finalize()
{
}

//---------------------------------------------------------
// �ǂƂ̉����o������
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 EnemySearch::CheckHitAndResolve(
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