/*
  @file BomMovement.cpp
  @brief �{���ړ���ԃN���X
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Libraries/MyLib/DebugString.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Wall/Wall.h"



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
	,m_gravity(0.0f, BomState::GRAVITY, 0.0f)
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
	
	// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//���f�������[�h����
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("Bom").c_str(), *fx);
	// �o�E�f�B���O�X�t�B�A�̐ݒ�
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = BomState::BOUNDINGSPHERERADIUS;

#ifdef _DEBUG	// �f�o�b�N�����s
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	//�f�o�b�N�p�̃R���W����
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
#endif
}

//---------------------------------------------------------
// ���O�X�V
//---------------------------------------------------------
void BomMovement::PreUpdate()
{
	//���݂̈ʒu���擾
	m_position = m_bomState->GetPosition();
	// �J�E���g�_�E���̎��Ԃ�ݒ�
	m_bomState->SetExplosionTimer(COUNTDOWNTIME);
	// ���݌����Ă�������̎擾
	Projection(m_player->GetForwardDirection());
	// ���x�̐ݒ�
	m_bomState->SetVelocity(m_velocity);
	// �o�E���X�����Ă��Ȃ��ɐݒ�
	m_bomState->SetBounce(false);
	// ���݂̏�Ԃ��ړ��ɂ���
	m_bomState->SetBomPresent(BomState::BomtPresent::MOVEMENT);
}

//---------------------------------------------------------
// ���˓���
//---------------------------------------------------------
void BomMovement::Projection(const DirectX::SimpleMath::Vector3& playerForwardDirection)
{
	// ���e�̏����x�̐ݒ�i�p�x�����߂�j
	float angleDeg = m_bomState->GetAngle();                    
	// ���W�A���ɕϊ�
	float angleRad = DirectX::XMConvertToRadians(angleDeg);
	// �����x�x�N�g���̌v�Z
	float vx = LAUNCHSPEED * std::cos(angleRad);           // ���������̑��x����
	float vy = LAUNCHSPEED * std::sin(angleRad);           // ���������̑��x����
	// �����x�x�N�g���̐ݒ�
	m_velocity = playerForwardDirection * vx + DirectX::SimpleMath::Vector3(0.0f, vy, 0.0f);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void BomMovement::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	// �o�E�f�B���O�X�t�B�A�̐ݒ�
	m_bomState->SetBoundingSphere(m_boundingSphere);
	// �����蔻��̈ʒu�̍X�V
	m_boundingSphere.Center = m_position;
	// ��葬�x�̏㏑��
	m_velocity = m_bomState->GetVelocity();
	// �d�͂̍X�V
	m_velocity += m_gravity * elapsedTime;
	// �n�ʂɒ��n�����ꍇ�AY�����̑��x�𔽓]������
	if (m_position.y < GROUNDHEIGHT){
		// �o�E���X��Ԃɂ���
		m_bomState->SetBounce(true);
		// ���x���[���ɂ���
		m_velocity = DirectX::SimpleMath::Vector3::Zero;
	}
	//�o�E���X���Ă��邩
	if (m_bomState->GetBounce()){
		// �o�E���X����������
		HandleCollision();
	}
	//�ǂƂ̓����蔻��
	for (const auto& wall : m_wall){
		HitCheck(wall->GetBoundingBox(),wall->GetExist());
	}
	// �ʒu�̍X�V
	m_position += m_velocity * elapsedTime;
	//0��艺�ɂ����Ȃ��悤��
	if (m_position.y < MINHEIGHT){
		m_position.y = MINHEIGHT;
	}
	//���Ԃ������甚��������
	if (m_bomState->GetExplosionTimer() < 0.0f){
		// ������ԂɈڍs
		m_bomState->ChangeState(m_bomState->GetBomExplosion());
	}
	//�@�o�E�f�B���O�X�t�B�A�̈ʒu���X�V
	m_boundingSphere.Center = m_position;
	// �ʒu���X�V����
	m_bomState->SetPosition(m_position);
	// ���x�̍X�V
	m_bomState->SetVelocity(m_velocity);
}

// ����X�V����
void BomMovement::PostUpdate()
{
	//�ʒu�̍X�V
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
	Matrix world = DirectX::SimpleMath::Matrix::CreateScale(BomState::BOMBMODELSIZE) * 
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
	m_velocity = -m_velocity * BOUNCEFACTOR;
	// �o�E���X���Ă��Ȃ���Ԃɂ���
	m_bomState->SetBounce(false);
}

//---------------------------------------------------------
// �ǂƂ̏Փ˔��肷��
//---------------------------------------------------------
void BomMovement::HitCheck(DirectX::BoundingBox boundingBox, const bool IsWall)
{
	using namespace DirectX::SimpleMath;

	// �o�E���f�B���O�X�t�B�A�ƕǂ�AABB���Փ˂��Ă��邩���`�F�b�N
	bool isHitWall = m_boundingSphere.Intersects(boundingBox);
	// ���Ă��Ȃ��Ȃ��΂�
	if (!isHitWall || !IsWall ) { return; }
	// �Փˎ��̑��x�x�N�g�����v�Z����
	Vector3 bMin = boundingBox.Center - boundingBox.Extents;
	Vector3 bMax = boundingBox.Center + boundingBox.Extents;
	// �����Ԃ��̐��l�����邽�߂̕�
	Vector3 normal = Vector3::Zero;
	// X�����̏Փ�
	if (m_position.x < bMin.x || m_position.x > bMax.x) {
		normal.x = (m_position.x < bMin.x) ? 1.0f : -1.0f;
	}
	// Y�����̏Փ�
	else if (m_position.y < bMin.y || m_position.y > bMax.y){
		normal.y = (m_position.y < bMin.y) ? 1.0f : -1.0f;
	}
	// Z�����̏Փ�
	else if (m_position.z < bMin.z || m_position.z > bMax.z){
		normal.z = (m_position.z < bMin.z) ? 1.0f : -1.0f;
	}
	// ���˃x�N�g�����v�Z����(�������˂����邽�߂ɂQ�{�j
	Vector3 reflection = m_velocity - 2.0f * (m_velocity.Dot(normal)) * normal;
	// ���˃x�N�g����ݒ肷��
	m_velocity = reflection * BOUNCEFACTOR;
	// �Փˌ�̈ʒu�����i�ʒu�����������߂��j
	m_position -= normal * 0.01f; // ���ꂵ�Ȃ��Ƃ߂�����s�R�ȓ�������

}