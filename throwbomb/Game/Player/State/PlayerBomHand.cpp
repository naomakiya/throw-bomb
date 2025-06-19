/*
  @file PlayerBomHand.cpp
  @brief �v���C���{�������N���X
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/Player/State/PlayerBomHand.h"
#include "Game/Player/State/PlayerIdling.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/DebugString.h"
#include "Game/Wall/Wall.h"
#include "Game/Camera/Camera.h"
#include "Game/Bom/BomState.h"

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
PlayerBomHand::PlayerBomHand(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall)
	: m_commonResources(nullptr)
	, m_camera(camera)
	, m_wall(wall)
	, m_player(playerState)
	, m_position(DirectX::SimpleMath::Vector3::Zero)
	, m_rotate(DirectX::SimpleMath::Quaternion::Identity)
	, m_handHeight(0.0f)
	, m_time(0.0f)
{
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
PlayerBomHand::~PlayerBomHand()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void PlayerBomHand::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/Player");

	//���f�������[�h����
	m_playerface = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/face.cmo", *fx);
	m_playerfaceBody = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/Body.cmo", *fx);
	m_playerHandL = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/PlayerHand.cmo", *fx);
	m_playerHandR = DirectX::Model::CreateFromCMO(device, L"Resources/Models/Player/PlayerHand.cmo", *fx);

}

//---------------------------------------------------------
// ���O�X�V����
//---------------------------------------------------------
void PlayerBomHand::PreUpdate()
{
	m_position = m_player->GetPosition();
	m_rotate = m_player->GetAngle();
	
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void PlayerBomHand::Update(const float& elapsedTime)
{
	using namespace DirectX::SimpleMath;
	//���Ԍo��
	m_time += elapsedTime;
	// �L�[�{�[�h�̏�Ԃ��擾����
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	Vector3 velocity = Vector3::Zero;
	//���݂̈ʒu���l������
	m_position = m_player->GetPosition();

	// �ړ������̎擾
	if (kbState.Up) {
		velocity += Vector3(0.0f, 0.0f, 25.0f); // �O��
	}
	if (kbState.Down) {
		velocity += Vector3(0.0f, 0.0f, -25.0f); // ���
	}
	if (kbState.Left) {
		velocity += Vector3(25.0f, 0.0f, 0.0f); // ��
	}
	if (kbState.Right) {
		velocity += Vector3(-25.0f, 0.0f, 0.0f); // �E
	}


	// �ړ�����������ꍇ
	if (velocity.LengthSquared() > 0) 
	{
		velocity.Normalize(); // �x�N�g���𐳋K��
		Vector3 vel = { 2.5f,0.0f,2.5f };

		// �J�����̉�]���擾
		Quaternion cameraRotation = m_camera->GetRotation(); // �J�����̉�]���擾
		// �v���C���[�̈ړ��������J�����̌����ɕϊ�
		Vector3 moveDirection = Vector3::Transform(velocity, cameraRotation);

		// �v���C���[�̌������X�V
		float yaw = atan2f(moveDirection.x, moveDirection.z);
		m_rotate = Quaternion::CreateFromYawPitchRoll(yaw, 0.0f, 0.0f); // �v���C���[����������

		// �ړ��ʂ��v�Z
		moveDirection *= vel; // ������̑��x���ړ��ɓK�p

		// �v���C���[�̈ʒu���X�V
		m_position += moveDirection * elapsedTime;
	}

	// �v���C���[�̈ʒu���X�V
	this->PostUpdate();

}

//---------------------------------------------------------
// ����X�V����
//---------------------------------------------------------
void PlayerBomHand::PostUpdate()
{
	// �v���C���[�̈ʒu���X�V
	m_player->SetPosition(m_position);
	// �v���C���[�̌������X�V
	m_player->SetAngle(m_rotate);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void PlayerBomHand::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();
	
	m_handHeight = 0.75f;
	// ��̗h����v�Z
	float faceSwayAngle = std::sin(m_time * 1.5f) * 0.35f;
	Quaternion faceRotation = Quaternion::CreateFromAxisAngle(Vector3::Up, faceSwayAngle);

	// �v���C���[�̌����ɑ΂��č��E�Ɏ���L���邽�߂̃x�N�g�����`
	Vector3 handOffsetL = Vector3(0.35f, 0.0f, 0.0f);  // ����̃I�t�Z�b�g
	Vector3 handOffsetR = Vector3(-0.35f, 0.0f, 0.0f); // �E��̃I�t�Z�b�g
	Vector3 legOffsetL(0.5f, -0.5f, 0.0f);
	Vector3 legOffsetR(-0.5f, -0.5f, 0.0f);

	// ��̌������v���C���[�̉�]�ɕ��s�ɂ���
	handOffsetL = Vector3::Transform(handOffsetL, m_rotate);  // �v���C���[�̉�]�ɉ���������̃I�t�Z�b�g
	handOffsetR = Vector3::Transform(handOffsetR, m_rotate);  // �v���C���[�̉�]�ɉ������E��̃I�t�Z�b�g
	legOffsetL = Vector3::Transform(legOffsetL, m_rotate);
	legOffsetR = Vector3::Transform(legOffsetR, m_rotate);

	// ���[���h�s��̌v�Z
	Matrix faceWorld = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate * faceRotation) * Matrix::CreateTranslation(m_position);
	Matrix bodyWorld = Matrix::CreateScale(m_player->PLAYERMODLSCALE) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_position);

	// ��̃��[���h�s����쐬
	Matrix handL = Matrix::CreateScale(m_player->PLAYERMODLSCALE)
		* Matrix::CreateTranslation(m_position + handOffsetL + Vector3(0.0f, m_handHeight, 0.0f)); // ����̈ʒu
	Matrix handR = Matrix::CreateScale(m_player->PLAYERMODLSCALE)
		* Matrix::CreateTranslation(m_position + handOffsetR + Vector3(0.0f, m_handHeight, 0.0f)); // �E��̈ʒu


	//���̃��[���h�s��
	Matrix legL = Matrix::CreateScale(0.0085f) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_position + legOffsetL);
	Matrix legR = Matrix::CreateScale(0.0085f) * Matrix::CreateFromQuaternion(m_rotate) * Matrix::CreateTranslation(m_position + legOffsetR);

	if(!m_player->GetHitEnemy())
	{
		// ���f����\������
		m_playerface->Draw(context, *states, faceWorld, view, projection);
		m_playerfaceBody->Draw(context, *states, bodyWorld, view, projection);
		m_playerHandL->Draw(context, *states, handL, view, projection);
		m_playerHandR->Draw(context, *states, handR, view, projection);
		m_playerHandL->Draw(context, *states, legL, view, projection);
		m_playerHandR->Draw(context, *states, legR, view, projection);
	}
	else
	{
		m_player->EnemyHitFlashing(*m_playerface, faceWorld, view, projection);
		m_player->EnemyHitFlashing(*m_playerfaceBody, bodyWorld,view,projection);
	}

}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void PlayerBomHand::Finalize()
{
}

//---------------------------------------------------------
// �ړ�
//---------------------------------------------------------
void PlayerBomHand::Movement(const float& elapsedTime)
{
	using namespace DirectX::SimpleMath;
	// �L�[�{�[�h�X�e�[�g���擾����
	DirectX::Keyboard::State kbState = DirectX::Keyboard::Get().GetState();
	// ����
	Vector3 velocity = Vector3::Zero;

	// ���[���A�s�b�`�A���[
	float roll = 0.0f;	// �y����]
	float pitch = 0.0f;	// �w����]
	float yaw = 0.0f;	// �x����]

	if (kbState.Left)	yaw = DirectX::XMConvertToRadians(1.0f);
	if (kbState.Right)	yaw = DirectX::XMConvertToRadians(-1.0f);

	// �N�H�[�^�j�I�������[���A�s�b�`�A���[�̒l���琶������
	Quaternion q = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	// ��]
	m_rotate = q * m_rotate;
	// ���K������
	velocity.Normalize();

	// �ړ��ʂ��v�Z����
	velocity *= m_player->SPEED;;

	// �v���C���[�̈ʒu���X�V����
	m_position += velocity * elapsedTime;

	// �v���C���[�̈ړ������݂�
	Vector3 newPosition = m_position + velocity;

	// �v���C���[�̈ʒu���X�V����
	m_position = newPosition;
	// ���W���X�V����
	if (kbState.Up)		m_position += Vector3::Transform(Vector3(0.0f, 0.0f, 0.1f), m_rotate);
	if (kbState.Down)	m_position -= Vector3::Transform(Vector3(0.0f, 0.0f, 0.1f), m_rotate);
}
