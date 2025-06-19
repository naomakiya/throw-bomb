/*
  @file Player.cpp
  @brief �v���C���N���X
*/
#include "pch.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemySearch.h"
#include"Game/Enemy/StraighteningEnemyState.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemyDash.h"
#include"Framework/CommonResources.h"
#include "Framework/Resources.h"
#include"Game/Wall/Wall.h"
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <DirectXCollision.h>
#include"Interface/IEnemyState.h"
#include"Game/Player/PlayerState.h"
#include"Libraries/MyLib/CollisionDebugRenderer.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
 StraighteningEnemySearch:: StraighteningEnemySearch(StraighteningEnemyState* enemyState,
    const std::vector<std::unique_ptr<Wall>>& wall,PlayerState* player )
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_EnemyModel{},
	m_model{},
	m_position{},
    m_wall{wall},
	 m_stoptime{0},
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale()),
	m_rotation{ DirectX::SimpleMath::Quaternion::Identity },
	m_player{player},
	m_collisionDebugRenderer{},
	m_searchBoundingBox{}
{

}

 StraighteningEnemySearch::~ StraighteningEnemySearch()
{
}

void  StraighteningEnemySearch::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	m_position = pos;
	m_position.z = pos.z + 0.5f; //�{�b�N�X�̈ʒu��O��0.5f�ړ������邷��

	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
	
	// �o�E���f�B���O�{�b�N�X�̃T�C�Y��ݒ�i��2.0�A����2.0�A���s��2.0�̗����́j
	m_searchBoundingBox = DirectX::BoundingOrientedBox(pos, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.5f), DirectX::SimpleMath::Quaternion::Identity);
}


void  StraighteningEnemySearch::PreUpdate()
{
	m_position = m_enemy->GetPosition();
	m_boundingBox = m_enemy->GetBoundingBox();
	m_boundingBox.Center = m_position;
}

void  StraighteningEnemySearch::Update(const float& elapsedTime)
{
	this->PreUpdate();
	// 1�b��15�x��]����
	float rotationSpeed = DirectX::XMConvertToRadians(15.0f) * elapsedTime;

	// Y����]�̃N�H�[�^�j�I�����쐬���A���݂̉�]�ɓK�p
	DirectX::SimpleMath::Quaternion rotationDelta = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, rotationSpeed);
	m_rotation = m_rotation * rotationDelta;
	m_rotation.Normalize(); // ���K��

	// �G�̑O���x�N�g�����擾
	DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Forward;
	forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation));

	// �T���p�̔��̍X�V
	
	// �O���ɔz�u
	m_searchBoundingBox.Center = m_position + forward * -1.5f;  
	// ��]��K�p
	m_searchBoundingBox.Orientation = m_rotation; 

	if (m_searchBoundingBox.Intersects(m_player->GetBoundingBox()))
	{
		m_enemy->ChangeState(m_enemy->GetStraighteningEnemyDash());
	}
}

void  StraighteningEnemySearch::PostUpdate()
{
    m_enemy->SetPosition(m_position);
}

void  StraighteningEnemySearch::Render(ID3D11DeviceContext* context,
	const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
	auto states = m_commonResources->GetCommonStates();

	// ��]���N�H�[�^�j�I������}�g���N�X�֕ϊ�
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation);

	// ���[���h�s����쐬
	DirectX::SimpleMath::Matrix world =
		DirectX::SimpleMath::Matrix::CreateScale(0.006) *
		rotationMatrix *
		DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

	//�ԐF�ŕ`��
	const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
		DirectX::Colors::Red.f[1],
		DirectX::Colors::Red.f[2],
		DirectX::Colors::Red.f[3]);

	// ���f����\������
	model.Draw(context, *states, world, view, projection);

}

	

void  StraighteningEnemySearch::Finalize()
{
}

