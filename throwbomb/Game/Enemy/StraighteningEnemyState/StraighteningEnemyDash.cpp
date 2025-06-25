/*
  @file StraighteningEnemyDash.cpp
  @brief �G�l�~�[�ːi�N���X
*/
#include "pch.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemyDash.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemySearch.h"
#include"Game/Enemy/StraighteningEnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Wall/Wall.h"
#include"Interface/IEnemyState.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Player/PlayerState.h"
#include <Libraries/Microsoft/ReadData.h>


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
StraighteningEnemyDash::StraighteningEnemyDash(StraighteningEnemyState* enemyState,
    const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_EnemyModel{},
	m_model{},
	m_position{},
    m_wall{wall},
    m_dashTime{0.0f},
    m_isHit{true},
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale()),
    m_player{player},
    m_boundingBox{},
    m_direction{}
{

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
StraighteningEnemyDash::~StraighteningEnemyDash()
{
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void StraighteningEnemyDash::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
   
	assert(resources);
	m_commonResources = resources;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

    m_position = pos;

    m_boundingBox = m_enemy->GetBoundingBox();

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
//���O�X�V
//---------------------------------------------------------
void StraighteningEnemyDash::PreUpdate()
{
    m_dashTime = 0.0f;
    m_position = m_enemy->GetPosition();
    m_boundingBox = m_enemy->GetBoundingBox();
}

//---------------------------------------------------------
// �X�V
//---------------------------------------------------------
void StraighteningEnemyDash::Update(const float& elapsedTime)
{
    m_dashTime += elapsedTime;

    //�����x�N�g���̌v�Z
    DirectionVectorCalculation();

    if (m_dashTime < 2.0f) return; //1�b�ȉ��Ȃ��΂�

    // �X�^�[�g�_�b�V���F�ŏ���0.5�b�Ԃ͋}����
    float speed = (m_dashTime - 1.0f < 0.5f) ? 15.0f : 5.0f;

    // Y���W��ω������Ȃ��悤�ɂ���
    m_position.x += m_direction.x * speed * elapsedTime;
    m_position.z += m_direction.z * speed * elapsedTime;

    m_boundingBox.Center = m_position;

    // �ːi���Ԃ�2�b�𒴂�����I������ `Search` ��Ԃֈڍs
    if (m_dashTime >= 2.5f)m_enemy->ChangeState(m_enemy->GetStraighteningEnemySearch());

    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
}


//---------------------------------------------------------
// ����X�V
//---------------------------------------------------------
void StraighteningEnemyDash::PostUpdate()
{
   
}

//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void StraighteningEnemyDash::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    using namespace DirectX::SimpleMath;

    auto states = m_commonResources->GetCommonStates();

    // ���[���h�s����X�V����
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006f);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation)
          * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

    
    //�G�̕`��
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

void StraighteningEnemyDash::Finalize()
{
}

//---------------------------------------------------------
// �����x�N�g���̌v�Z
//---------------------------------------------------------
void StraighteningEnemyDash::DirectionVectorCalculation()
{
    // �v���C���[�ւ̕����x�N�g�����v�Z
    m_direction = m_player->GetPosition() - m_position;

    //m_direction.Length(); // �x�N�g���̒������擾
    m_direction.Normalize();

    // �i�s�������������߂̉�]���v�Z
    m_rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
        atan2(m_direction.x, m_direction.z),  // Yaw (Y����])
        0.0f,  // Pitch
        0.0f   // Roll
    );
}