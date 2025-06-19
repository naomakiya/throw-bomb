/*
  @file  Wall.cpp
  @brief �ǃN���X
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Wall/Wall.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"

float Wall::THRESHOLD = 0.00f; 

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
Wall::Wall(WallType wallType, std::shared_ptr<DirectX::Model> model, const DirectX::SimpleMath::Vector3& position)
	:
	m_commonResources{ nullptr },
	m_collisionDebugRenderer{ nullptr },
	m_depthStencilState{ nullptr },
	m_texture{ nullptr },
	m_wallType{ wallType },
	m_position{ position },
	m_boundingBox{},
	m_scale{ 1.0f },
	m_modelScale{ 0.01f },	//�P�O�O���̂P�̑傫��
	m_exist{ true },
	m_isVisible{ false },
	m_wallModel{ std::move(model) }
{
	
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
Wall::~Wall()
{
	Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void Wall::Initialize(CommonResources* resources)
{

	assert(resources);  // null�|�C���^�łȂ����m�F
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// �o�E���f�B���O�{�b�N�X�̐ݒ�
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = DirectX::SimpleMath::Vector3(m_scale);

	// �[�x�X�e���V���ݒ���\�z
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = true;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// �X�e���V���̐ݒ�: ���ʂ̏����Ɠ���
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace = desc.FrontFace;  // ���ʂ������ݒ�

	// �X�e���V����Ԃ��쐬
	DX::ThrowIfFailed(device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf()));
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void Wall::Render(ID3D11DeviceContext* context,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	
	// �[�x�X�e���V���X�e�[�g�ݒ�
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	// ���[���h�s��̍쐬
	Matrix world = Matrix::CreateScale(m_modelScale) * Matrix::CreateTranslation(m_position) ;

	// ���f���̕`��
	switch (m_wallType)
	{
	//�j��s�̕ǂ̕`��
	case WallType::Indestructible:
		m_wallModel->Draw(context, *m_commonResources->GetCommonStates(), world, view, projection);
		break;
	//�j��\�̕ǂ̕`��
	case WallType::Destructible:
		m_wallModel->Draw(context, *m_commonResources->GetCommonStates(), world, view, projection);
		break;
	default:
		break;
	}
	
#ifdef _DEBUG
	const Vector4 color(DirectX::Colors::Red.f[0], DirectX::Colors::Red.f[1], DirectX::Colors::Red.f[2], DirectX::Colors::Red.f[3]);
	m_collisionDebugRenderer->RenderBoundingBox(m_boundingBox, view, projection, color);
#endif
	
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void Wall::Finalize()
{
	// �[�x�X�e���V���X�e�[�g�̉��
	if (m_depthStencilState) {
		m_depthStencilState.Reset();
	}
}

//---------------------------------------------------------
// �ǂƂ̓����蔻��
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 Wall::CheckHitAndResolve(const DirectX::BoundingBox& a, const DirectX::BoundingBox& b)
{
	using namespace DirectX::SimpleMath;

	// �o�E���f�B���O�{�b�N�X���m�̏Փ˔�����Ƃ� || �����Ă���Ȃ�A�Փ˂Ȃ�
	if (!a.Intersects(b) || m_isVisible) { return Vector3::Zero; }

	// AABB�p��min/max���v�Z����
	Vector3 aMin = a.Center - a.Extents;
	Vector3 aMax = a.Center + a.Extents;
	Vector3 bMin = b.Center - b.Extents;
	Vector3 bMax = b.Center + b.Extents;

	// �e���̍������v�Z����
	float dx1 = bMax.x - aMin.x;
	float dx2 = bMin.x - aMax.x;
	float dy1 = bMax.y - aMin.y;
	float dy2 = bMin.y - aMax.y;
	float dz1 = bMax.z - aMin.z;
	float dz2 = bMin.z - aMax.z;

	// �e���ɂ��āA��Βl�̏������������̂߂荞�ݗʂƂ���FAABB�̏d�Ȃ�����������肷��
	float dx = std::abs(dx1) < std::abs(dx2) ? dx1 : dx2;
	float dy = std::abs(dy1) < std::abs(dy2) ? dy1 : dy2;
	float dz = std::abs(dz1) < std::abs(dz2) ? dz1 : dz2;

	// �Փˎ��̉����߂��x�N�g��
	Vector3 pushBackVec = Vector3::Zero;

	// �Փ�臒l�𒴂����ꍇ�̂݉����߂�
	if (std::abs(dx) > THRESHOLD || std::abs(dy) > THRESHOLD || std::abs(dz) > THRESHOLD)
	{
		// �܂�X�������ł̏���
		if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz))
		{
			pushBackVec.x = dx; // X�������ŉ����߂�
		}
		// ����Z�������ł̏���
		else if (std::abs(dz) <= std::abs(dy))
		{
			pushBackVec.z = dz; // Z�������ŉ����߂�
		}
	}

	pushBackVec.y = dy;

	if (dy < 0.0f)
	{
		pushBackVec.y = 0.0f;
	}

	// �ʒu�������݂ɐU������h�����߁A�����ȏC���ʂ𖳎�
	if (std::abs(pushBackVec.x) < 0.10f) { pushBackVec.x = 0.0f; }
	///*if (std::abs(pushBackVec.y) < 0.10f) { pushBackVec.y = 0.0f; }*/
	if (std::abs(pushBackVec.z) < 0.10f) { pushBackVec.z = 0.0f; }

	// �߂荞�݂̍��̉����Ԃ�
	return pushBackVec;  
}

