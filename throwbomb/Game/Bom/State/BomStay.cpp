/*
  @file  BomStay.cpp
  @brief �{���̐^���ɒu���N���X
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include <Game/ResourceManager/ResourceManager.h>
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomStay.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Bom/State/BomMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Wall/Wall.h"


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
BomStay::BomStay(BomState* bomState,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	: m_player(playerState)
	, m_commonResources(nullptr)
	, m_bomState(bomState)
	, m_wall(wall)
	, m_position{}
	, m_velocity{}
	, m_gravity(0.0f, BomState::GRAVITY, 0.0f)
	, m_boundingSphere{} 
	, m_bomModel(nullptr)
{
	
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
BomStay::~BomStay()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void BomStay::Initialize(CommonResources* resources)
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
	m_boundingSphere.Radius = BomState::BOUNDINGSPHERERADIUS;

}

// ���O�X�V����
void BomStay::PreUpdate()
{
	//	�u����ԂɕύX����
	m_bomState->SetBomPresent(STAY);
	// ���݂̈ʒu���擾����
	m_position = m_bomState->GetPosition();
	//��������܂ł̎���
	if(m_bomState->GetExplosionTimer() < 0.0f)m_bomState->SetExplosionTimer(5.0f);
	
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void BomStay::Update(const float& elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	// �d�͂̍X�V
	m_velocity += m_gravity * elapsedTime;   
	// �ʒu�̍X�V
	m_position += m_velocity * elapsedTime;

	//�ǂ̓����蔻��
	for (const auto& wall : m_wall){
		HitCheck(wall->GetBoundingBox(),wall->GetExist());
	}
	
	// �n�ʂɂ߂荞�܂Ȃ��悤�ɂ���
	if (m_position.y < BomMovement::GROUNDHEIGHT) m_position.y = BomMovement::GROUNDHEIGHT;

	//�@�����蔻��𒲐�����
	m_boundingSphere.Center = m_position;
	//���Ԃ������甚���ֈڍs������
	if (m_bomState->GetExplosionTimer() < 0.0f){
		// ������ԂɈڍs
		m_bomState->ChangeState(m_bomState->GetBomExplosion());
	}
}

// ����X�V����
void BomStay::PostUpdate()
{
	// �ʒu���̍X�V
	m_bomState->SetPosition(m_position);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void BomStay::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();
	
	// ���[���h�s����X�V����
	Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.005) * 
		Matrix::CreateTranslation(m_position);
	// ���f����\������
	m_bomModel->Draw(context, *states, world, view, projection);

}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void BomStay::Finalize()
{
}

//---------------------------------------------------------
// �Փ˔��肷��
//---------------------------------------------------------
void BomStay::HitCheck(DirectX::BoundingBox boundingBox, const bool IsWall)
{
	using namespace DirectX::SimpleMath;

	// �o�E���f�B���O�X�t�B�A�ƕǂ�AABB���Փ˂��Ă��邩���`�F�b�N
	bool isHitWall = m_boundingSphere.Intersects(boundingBox);
	// �������ĂȂ��Ȃ��΂�
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

	// �Փˌ�̈ʒu�����i�ʒu�����������߂��j
	m_position -= normal * 0.01f; // �K�X����
	
	m_velocity = Vector3(0.0f,0.0f,0.0f);

}