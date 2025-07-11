/*
  @file  PlayerMovement.cpp
  @brief �v���C���ړ���ԃN���X
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Player/State/PlayerMovement.h"
#include "Game/Player/State/PlayerBomHand.h"
#include "Game/Player/State/PlayerIdling.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/Wall/Wall.h"
#include "Game/Camera/Camera.h"
#include "Game/Bom/BomState.h"
#include "Game/ResourceManager/ResourceManager.h"


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
PlayerMovement::PlayerMovement(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_postion{},
	m_player{ playerState },
	m_currentState{},
	m_playerHandL{},
	m_playerHandR{},
	m_commonResources{},
	m_wall{ wall },
	m_angle{ 0.0f },
    m_scale{ 0.0f },
	m_rotate{},
	m_camera{camera},
	m_hand{0.0f},
	m_handAngle{0.0f},
	m_time{0.0f},
	m_vel{0}
{
	
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
PlayerMovement::~PlayerMovement()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void PlayerMovement::Initialize(CommonResources* resources)
{
	m_commonResources = resources;
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	
	// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/Player");
	//���f�������[�h����
	m_playerface = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerFace").c_str(), *fx);
	m_playerfaceBody = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerBody").c_str(), *fx);
	m_playerHandL = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerHand").c_str(), *fx);
	m_playerHandR = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("PlayerHand").c_str(), *fx);

}

//���O����
void PlayerMovement::PreUpdate()
{
	// �O�̏�Ԃ̈ʒu�����擾
	m_postion = m_player->GetPosition();
	// �O�̏�Ԃ̕��������擾
	m_rotate = m_player->GetAngle();
	// ���x������
	m_vel = 0.0f;
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void PlayerMovement::Update(const float& elapsedTime)
{
	//�@�ʒu���̍X�V
	m_postion = m_player->GetPosition();
	// ��]���x�𒲐�
	m_handAngle = elapsedTime * 5.0f;
	//�ړ�
	Movement(elapsedTime);
	// �v���C���[�̍X�V
	PostUpdate();
}

//---------------------------------------------------------
// ���O�X�V����
//---------------------------------------------------------
void PlayerMovement::PostUpdate()
{
	// �v���C���[�̈ʒu���X�V
	m_player->SetPosition(m_postion);
	m_player->SetAngle(m_rotate);

}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void PlayerMovement::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	//�ǂɂȂǂ̐G�ꂽ��̈ʒu�����擾
	m_postion = m_player->GetPosition();

	// ���[���h�s����X�V����
	Matrix world = Matrix::CreateScale(m_player->PLAYERMODLSCALE);
	world *= Matrix::CreateFromQuaternion(m_rotate);
	world *= Matrix::CreateTranslation(m_postion);

	// ��
	Matrix world2 = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateTranslation(m_postion);

	// ��̐U�蕝�Ɗp�x���v�Z
	float handRadius = 0.7f; // �~�̔��a

	// ��̃I�t�Z�b�g�v�Z�i
	Vector3 handOffsetL(handRadius * std::cos(m_handAngle),  handRadius * std::sin(m_handAngle), handRadius * std::cos(m_handAngle));
	Vector3 handOffsetR(handRadius * std::cos(m_handAngle + DirectX::XM_PI), std::max(0.0f, handRadius * std::sin(m_handAngle + DirectX::XM_PI)), 0.1f);

	// ��̌������v���C���[�̉�]�ɕ��s�ɂ���
	handOffsetL = Vector3::Transform(handOffsetL, m_rotate);
	handOffsetR = Vector3::Transform(handOffsetR, m_rotate);

	// ��̃��[���h�s����쐬
	Matrix handL = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateTranslation(m_postion + handOffsetL);
	Matrix handR = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateTranslation(m_postion + handOffsetR);

	// ���̃I�t�Z�b�g�ƃ��[���h�s��v�Z
	Vector3 legOffsetL = Vector3(0.5f, -0.5f, 0.0f);
	Vector3 legOffsetR = Vector3(-0.5f, -0.5f, 0.0f);

	legOffsetL = Vector3::Transform(legOffsetL, m_rotate);
	legOffsetR = Vector3::Transform(legOffsetR, m_rotate);

	Matrix legL = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_postion + legOffsetL);
	Matrix legR = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_postion + legOffsetR);

	if (!m_player->GetHitEnemy()){
		// ���f����\������
		m_playerface->Draw(context, *states, world, view, projection);
		m_playerfaceBody->Draw(context, *states, world2, view, projection);
		m_playerHandL->Draw(context, *states, handL, view, projection);
		m_playerHandR->Draw(context, *states, handR, view, projection);
		m_playerHandL->Draw(context, *states, legL, view, projection);
		m_playerHandR->Draw(context, *states, legR, view, projection);
	}
	else{
		// �����ɂ���
		m_player->EnemyHitFlashing(*m_playerface, world, view, projection);
		m_player->EnemyHitFlashing(*m_playerfaceBody, world2, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandL, handL, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandR, handR, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandL, legL, view, projection);
		m_player->EnemyHitFlashing(*m_playerHandR, legR, view, projection);
	}
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void PlayerMovement::Finalize()
{
}

//---------------------------------------------------------
//�ړ�����
//---------------------------------------------------------
void PlayerMovement::Movement(const float& elapsedTime)
{
	using namespace DirectX::SimpleMath;
	// �L�[�{�[�h�̏�Ԃ��擾����
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	Vector3 velocity = Vector3::Zero;

	// �ړ������̎擾
	if (kbState.Up) {
		velocity += Vector3(0.0f, 0.0f, 1.0f); // �O��
	}
	if (kbState.Down) {
		velocity += Vector3(0.0f, 0.0f, -1.0f); // ���
	}
	if (kbState.Left) {
		velocity += Vector3(1.0f, 0.0f, 0.0f); // ��
	}
	if (kbState.Right) {
		velocity += Vector3(-1.0f, 0.0f, 0.0f); // �E
	}

	// �ړ�����������ꍇ
	if (velocity.LengthSquared() > 0)
	{
		// �x�N�g���𐳋K��
		velocity.Normalize(); 
		// ������K�p�i���x�̑����j
		m_vel += acceleration; // m_vel�i���x�j�ɉ����x��������
		// �ő呬�x�𒴂��Ȃ��悤�ɐ���
		if (m_vel > MAXSPEED) { m_vel = MAXSPEED; }
		// �J�����̉�]���擾
		Quaternion cameraRotation = m_camera->GetRotation(); 
		// �v���C���[�̈ړ��������J�����̌����ɕϊ�
		Vector3 moveDirection = Vector3::Transform(velocity, cameraRotation);
		// �v���C���[�̌������X�V
		float yaw = atan2f(moveDirection.x, moveDirection.z);
		m_rotate = Quaternion::CreateFromYawPitchRoll(yaw, 0.0f, 0.0f); // �v���C���[����������
		// �ړ��ʂ��v�Z
		moveDirection *= m_vel ; // ������̑��x���ړ��ɓK�p
		// �v���C���[�̈ʒu���X�V
		m_postion += moveDirection * elapsedTime;
	}

	// �v���C���[���ړ����Ă��Ȃ��ꍇ�̓A�C�h����Ԃɂ���
	if (kbState.Up == false && kbState.Down == false && !kbState.Left && !kbState.Right) {
		m_player->ChangeState(m_player->GetPlayerIdling());
	}
}
