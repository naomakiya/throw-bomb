/*
  @file BomMovement.cpp
  @brief �{���ړ���ԃN���X
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/Resources.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Wall/Wall.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Libraries/MyLib/DebugString.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
BomMovement::BomMovement(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	:m_commonResources(nullptr)
	,m_player(playerState)
	,m_bomState(bomState)
	,m_wall(wall)
	,m_position{}
	,m_velocity{}
	,m_gravity(0.0f, GravityY, 0.0f)
	,m_isBounce(false)
{
	
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
BomMovement::~BomMovement()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void BomMovement::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	//auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");

	//���f�������[�h����
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Bom").c_str(), *fx);

	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = BoundingSphereRadius;

#ifdef _DEBUG	// �f�o�b�N�����s
	//�f�o�b�N�p�̃R���W����
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
#endif
}

// ���O�X�V����
void BomMovement::PreUpdate()
{
	m_position = m_bomState->GetPosition();
	m_bomState->SetExplosionTimer(CountdownTime);
	Projection(m_player->GetForwardDirection());
	m_bomState->SetVelocity(m_velocity);
	m_bomState->SetBounce(false);
	m_bomState->SetBomPresent(BomState::BomtPresent::MOVEMENT);
	
}
//---------------------------------------------------------
//���˓���
//---------------------------------------------------------
void BomMovement::Projection(const DirectX::SimpleMath::Vector3& playerForwardDirection)
{
	// ���e�̏����x�̐ݒ�i�p�x�����߂�j
	float angleDeg = m_bomState->GetAngle();                    
	// ���W�A���ɕϊ�
	float angleRad = DirectX::XMConvertToRadians(angleDeg);

	// �����x�x�N�g���̌v�Z�i���������Ɛ��������̐����j
	float vx = LaunchSpeed * std::cos(angleRad);           // ���������̑��x����
	float vy = LaunchSpeed * std::sin(angleRad);           // ���������̑��x����

	// �����x�x�N�g���̐ݒ�
	m_velocity = playerForwardDirection * vx + DirectX::SimpleMath::Vector3(0.0f, vy, 0.0f);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void BomMovement::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	m_bomState->SetBoundingSphere(m_boundingSphere);

	m_boundingSphere.Center = m_position;
	m_velocity = m_bomState->GetVelocity();
	m_velocity += m_gravity * elapsedTime;
	
	// �n�ʂɒ��n�����ꍇ�AY�����̑��x�𔽓]������
	if (m_position.y < GroundHeight)
	{
		m_bomState->SetBounce(true);
		m_velocity = DirectX::SimpleMath::Vector3::Zero;
	}
	//�o�E���X���Ă��邩
	if (m_bomState->GetBounce())
	{
		HandleCollision();
	}
	//�ǂƂ̓����蔻��
	for (const auto& wall : m_wall)
	{
		CheckHit(wall->GetBoundingBox(),wall->GetExist());
	}

	// �ʒu�̍X�V
	m_position += m_velocity * elapsedTime;

	//0��艺�ɂ����Ȃ��悤��
	if (m_position.y < MinHeight)
	{
		m_position.y = MinHeight;
	}
	//���Ԃ������甚��������
	if (m_bomState->GetExplosionTimer() < 0.0f)
	{
		m_bomState->ChangeState(m_bomState->GetBomExplosion());
	}

	m_boundingSphere.Center = m_position;
	m_bomState->SetPosition(m_position);
	m_bomState->SetVelocity(m_velocity);
}

// ����X�V����
void BomMovement::PostUpdate()
{
	m_bomState->SetPosition(m_position);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void BomMovement::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();
	
	// ���[���h�s����X�V����
	Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.005f) * 
		Matrix::CreateTranslation(m_position);
	// ���f����\������
	m_bomModel->Draw(context, *states, world, view, projection);
#ifdef _DEBUG
	// �f�o�b�O�����uDebugString�v�ŕ\������
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("");
	debugString->AddString("BX : %f", m_position.x);
	debugString->AddString("BY : %f", m_position.y);
	debugString->AddString("BZ : %f", m_position.z);
	
	//�R���W�����̐F
	const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
		DirectX::Colors::Red.f[1],
		DirectX::Colors::Red.f[2],
		DirectX::Colors::Red.f[3]);
	//�f�o�b�O�`��
	m_collisionDebugRenderer->RenderBoundingSphere(m_boundingSphere, view, projection, color);
#endif

}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void BomMovement::Finalize()
{
}

//---------------------------------------------------------
//�{�����o�E���h����
//---------------------------------------------------------
void BomMovement::HandleCollision()
{
	// ���˃x�N�g����ݒ肷��
	m_velocity = -m_velocity * BounceFactor;

	m_bomState->SetBounce(false);
}

//---------------------------------------------------------
// �Փ˔��肷��
//---------------------------------------------------------
void BomMovement::CheckHit(DirectX::BoundingBox boundingBox, const bool IsWall)
{
	using namespace DirectX::SimpleMath;

	// �o�E���f�B���O�X�t�B�A�ƕǂ�AABB���Փ˂��Ă��邩���`�F�b�N
	bool isHitWall = m_boundingSphere.Intersects(boundingBox);

	if (!isHitWall || !IsWall ) { return; }

	// �Փˎ��̑��x�x�N�g�����v�Z����
	Vector3 bMin = boundingBox.Center - boundingBox.Extents;
	Vector3 bMax = boundingBox.Center + boundingBox.Extents;

	Vector3 normal = Vector3::Zero;

	if (m_position.x < bMin.x || m_position.x > bMax.x) // X�����̏Փ�
	{
		normal.x = (m_position.x < bMin.x) ? 1.0f : -1.0f;
	}
	else if (m_position.y < bMin.y || m_position.y > bMax.y) // Y�����̏Փ�
	{
		normal.y = (m_position.y < bMin.y) ? 1.0f : -1.0f;
	}
	else if (m_position.z < bMin.z || m_position.z > bMax.z) // Z�����̏Փ�
	{
		normal.z = (m_position.z < bMin.z) ? 1.0f : -1.0f;
	}

	// ���˃x�N�g�����v�Z����
	Vector3 reflection = m_velocity - 2.0f * (m_velocity.Dot(normal)) * normal;

	// ���˃x�N�g����ݒ肷��
	m_velocity = reflection * BounceFactor;

	// �Փˌ�̈ʒu�����i�ʒu�����������߂��j
	m_position -= normal * 0.01f; // �K�X����

}