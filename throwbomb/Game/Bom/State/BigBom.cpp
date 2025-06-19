/*
  @file Bom.cpp
  @brief �v���C���N���X
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include"Game/Bom/BigBom.h"
#include"Game/Scene/PlayScene.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/InputManager.h"
#include <cassert>
#include <algorithm>
#include"Framework/Graphics.h"
#include <Libraries/Microsoft/DebugDraw.h>
#include <chrono>
#include <thread>
#include"Game/Wall/Wall.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

//���˓���
void BigBom::SetExist( DirectX::SimpleMath::Vector3 playerPosition)
{
	// ���e�̐�����ݒ�
	m_exist = true;

	// ���e�̐����ʒu�̐ݒ�
	m_position = playerPosition;

	// �������Ԃ̃��Z�b�g
	m_explosionTime = 0.0f;
	m_exploded = false;
}


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
BigBom::BigBom(PlayScene* playScene, Graphics* graphics, const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_position{},
	m_playScene{ playScene },
	m_sphere{},
	m_commonResources{},
	m_mass{0},
	m_exist{false},
	m_gravity{ 0.0f, -9.81f, 0.0f }, // �d�͉����x 
	m_graphics(graphics),
	m_BoundingSphere{},
	m_context{},
	m_cnt{},
	m_cntTime{3},
	m_exploded{},
	m_explosionSphere{},
	m_explosionTime{},
	m_currentAngle{0},
	m_wall{wall}

{
	// �R���e�L�X�g���擾����
	m_context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
BigBom::~BigBom()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void BigBom::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = m_commonResources->GetCommonStates();

	// ���f����ǂݍ��ޏ���
	/*std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");*/

	// �W�I���g���b�N�v���~�e�B�u�𐶐�����
	m_sphere = GeometricPrimitive::CreateSphere(context);
	m_explosionSphere = GeometricPrimitive::CreateSphere(context);

	m_BoundingSphere.Center = m_position;
	m_BoundingSphere.Radius = 0.1f;

	//m_explosionSphere = std::make_unique<DirectX::BoundingSphere>(m_position, 3.0f); // �����̃X�t�B�A�R���C�_�[�̏�����
	m_explosionTime = 0.0f;  // �������Ԃ̃��Z�b�g
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void BigBom::Update(const float& elapsedTime)
{
	if (!m_exist) return;

	m_BoundingSphere.Center = m_position;

	if (m_exploded)
	{
		Explode(elapsedTime);
	}
	else
	{
		m_BoundingSphere.Radius = 0.1f;
	}
	
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void BigBom::Render(DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix projection)
{
	/*auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();*/
	// ���[���h�s����X�V����
	Matrix world = Matrix::CreateTranslation(m_position);

	Matrix worldExp = Matrix::CreateScale(DirectX::SimpleMath::Vector3(3.0f, 3.0f, 3.0f));
	worldExp *= Matrix::CreateTranslation(m_position);
	//���f����\������
	if (m_exist)
	{
		m_sphere->Draw(world, view, projection, Colors::Green);
	}
	if (m_exploded)
	{
		m_explosionSphere->Draw(worldExp, view, projection, Colors::Red);

	}

}

// �C�����E����`�悷��
void BigBom::DrawTurretBoundingSphere()
{
	// �v���~�e�B�u�`����J�n����
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// �o�E���f�B���O�X�t�B�A��`�悷��
	DX::Draw(m_graphics->GetPrimitiveBatch(), m_BoundingSphere, DirectX::Colors::White);

	// �v���~�e�B�u�`����I������
	m_graphics->DrawPrimitiveEnd();

}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void BigBom::Finalize()
{
}



// �����̋��E����`�悷��
void BigBom::DrawExplosionBoundingSphere()
{
	// �v���~�e�B�u�`����J�n����
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// �����̃o�E���f�B���O�X�t�B�A��`�悷��
	//DX::Draw(m_graphics->GetPrimitiveBatch(), *m_explosionSphere, DirectX::Colors::Red);

	// �v���~�e�B�u�`����I������
	m_graphics->DrawPrimitiveEnd();
}

//����
void BigBom::Explode(const float& elapsedTime)
{
	m_BoundingSphere.Radius = 2.5f;
	m_explosionTime += elapsedTime;
	m_exploded = true;
	if (m_explosionTime >= 1.0f)
	{
		m_exploded = false;
		m_isMove = false;
		m_exist = false;
	}
}

//---------------------------------------------------------
// �Փ˔��肷��
//---------------------------------------------------------
void BigBom::CheckHit(DirectX::BoundingBox boundingBox)
{
	bool isHitWall = false;
	isHitWall = m_BoundingSphere.Intersects(boundingBox);

	if (!isHitWall) { return; }

	// AABB�̍ŏ��l�ƍő�l���v�Z
	Vector3 bMin = boundingBox.Center - boundingBox.Extents;
	Vector3 bMax = boundingBox.Center + boundingBox.Extents;

	// �e���̍������v�Z
	float dx1 = bMax.x - m_BoundingSphere.Center.x + m_BoundingSphere.Radius;
	float dx2 = bMin.x - m_BoundingSphere.Center.x - m_BoundingSphere.Radius;
	float dy1 = bMax.y - m_BoundingSphere.Center.y + m_BoundingSphere.Radius;
	float dy2 = bMin.y - m_BoundingSphere.Center.y - m_BoundingSphere.Radius;
	float dz1 = bMax.z - m_BoundingSphere.Center.z + m_BoundingSphere.Radius;
	float dz2 = bMin.z - m_BoundingSphere.Center.z - m_BoundingSphere.Radius;

	// �e���ɂ��āA��Βl�̏������������̂߂荞�ݗʂƂ���
	float dx = std::abs(dx1) < std::abs(dx2) ? dx1 : dx2;
	float dy = std::abs(dy1) < std::abs(dy2) ? dy1 : dy2;
	float dz = std::abs(dz1) < std::abs(dz2) ? dz1 : dz2;

	// �����߂��x�N�g��
	Vector3 pushBackVec = Vector3::Zero;

	// �߂荞�݂���ԏ��������������߂�
	if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz)) {
		pushBackVec.x += dx;
	}
	else if (std::abs(dz) <= std::abs(dx) && std::abs(dz) <= std::abs(dy)) {
		pushBackVec.z += dz;
	}
	else {
		pushBackVec.y += dy;
	}

	// �����߂�
	m_position += pushBackVec;
	m_BoundingSphere.Center = m_BoundingSphere.Center + pushBackVec;

	if (pushBackVec.x > 0.0f || pushBackVec.y > 0.0f || pushBackVec.z > 0.0f)
	{
		m_exploded = true;
	}

}