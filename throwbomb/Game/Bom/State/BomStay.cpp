/*
  @file  BomStay.cpp
  @brief �{���̐^���ɒu���N���X
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/Resources.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomStay.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Player/PlayerState.h"
#include "Game/Wall/Wall.h"
#include <Game/ResourceManager/ResourceManager.h>

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
	, m_gravity(0.0f, -9.81f, 0.0f)
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
	m_bomModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Bom").c_str(), *fx);
	// �o�E�f�B���O�X�t�B�A�̐ݒ�
	m_boundingSphere.Radius = 0.25f;

}

// ���O�X�V����
void BomStay::PreUpdate()
{
	m_bomState->SetBomPresent(STAY);
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
	
	//�ǂ̓����蔻��
	for (const auto& wall : m_wall)
	{
		CheckHit(wall->GetBoundingBox(),wall->GetExist());
	}
	// �ʒu�̍X�V
	m_position += m_velocity * elapsedTime;  
	
	if (m_position.y < 0.5f) m_position.y = 0.5f;

	//�@�����蔻��𒲐�����
	m_boundingSphere.Center = m_position;
	//���Ԃ������甚���ֈڍs������
	if (m_bomState->GetExplosionTimer() < 0.0f)
	{
		m_bomState->ChangeState(m_bomState->GetBomExplosion());
	}
}

// ����X�V����
void BomStay::PostUpdate()
{
	m_bomState->SetBomPresent(EXPLOSION);
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
void BomStay::CheckHit(DirectX::BoundingBox boundingBox, const bool IsWall)
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

	// �Փˌ�̈ʒu�����i�ʒu�����������߂��j
	m_position -= normal * 0.01f; // �K�X����
	
	m_velocity = Vector3(0.0f,0.0f,0.0f);

}