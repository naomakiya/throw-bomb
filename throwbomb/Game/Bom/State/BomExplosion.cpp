/*
  @file BomExplosion.cpp
  @brief �{���̔�����ԃN���X
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Framework/Resources.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomExist.h"
#include "Game/Bom/State/BomExplosion.h"
#include "Game/Wall/Wall.h"
#include "Game/Sound/Sound.h"
#include <Libraries/Microsoft/ReadData.h>
#include "Game/Dithering/Dithering.h"
#include "Game/UI/Smork.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
BomExplosion::BomExplosion(BomState* bomState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
	:
	m_position{},
	m_bomState{ bomState },
	m_currentState{},
	m_explosionSphere{},
	m_commonResources{},
	m_wall{ wall },
    m_scale{0.25f},
	m_boundingSphere{},
	m_cnt{},
	m_player{ player },
	m_elapsedTime(0.0f),
	m_ps{},
	m_constantBuffer{},
	m_smork{}
{
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = m_scale;
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
BomExplosion::~BomExplosion()
{
	this->Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void BomExplosion::Initialize(CommonResources* resources)
{
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models/exprol");
	// �������̍쐬
	m_explosionSphere = DirectX::Model::CreateFromCMO(device, L"Resources/Models/exprol/exprol.cmo", *fx);

	// �������̃G�t�F�N�g��ݒ肷��
	m_explosionSphere->UpdateEffects([&](DirectX::DX11::IEffect* effect)
		{
			auto basicEffect = dynamic_cast<DirectX::DX11::BasicEffect*>(effect);
			if (basicEffect)
			{
				basicEffect->SetLightingEnabled(true);
				basicEffect->SetPerPixelLighting(true);
				basicEffect->SetTextureEnabled(true);
				basicEffect->SetVertexColorEnabled(false);
				basicEffect->SetFogEnabled(false);
			}
		}
	);
	//�V�F�[�_�[�̓ǂݎ��
	ShaderLoad(device);
	//�萔�o�b�t�@�[�̍쐬
	CreateConstanBuffer(device);

	m_dithering = std::make_unique<Dithering>(device);

	m_smork = std::make_unique<Smork>();
	m_smork->Create(m_commonResources->GetDeviceResources());

	m_explosionObjects = {
	{ Vector3(0, 0, 0), 1.0f, 0.0f, 0.8f, true },
	};

	// SpriteBatch ������
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

	// �e�N�X�`���[�ǂݍ���
	DX::ThrowIfFailed(
		DirectX::CreateWICTextureFromFile(
			device,
			L"Resources/Textures/Explosion.png",
			nullptr,
			m_explosionTexture.ReleaseAndGetAddressOf()
		)
	);
	
}

//���O����
void BomExplosion::PreUpdate()
{
	m_position = m_bomState->GetPosition();
	m_boundingSphere.Center = m_position;
	m_cnt = 2.0f;
	m_scale = 0.25f;
	m_elapsedTime = 0.0f;
	m_smork->SetOn(true);
	// SE���Đ�
	Sound::GetInstance().Initialize();
	Sound::GetInstance().PlaySE("Resources/Sounds/Bom.mp3");
	Sound::GetInstance().SetVolume(0.4f);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void BomExplosion::Update(const float& elapsedTime)
{
	m_elapsedTime += elapsedTime;
	//�J�E���g�_�E��
	m_cnt -= elapsedTime;
	//�@�ǂ�j�󂷂邩�m�F����
	CheckBreak();
	//  �p�[�e�B�N���̍X�V
	m_smork->Update(elapsedTime, m_position, m_scale);
	//���Ԍo�߂Ŗc�����x�𐧌�
	m_scale += 2.0f * elapsedTime;
	if (m_scale > 2.0)
	{
		m_scale = 2.0f;
	}
	//�@�傫����ύX����
	m_boundingSphere.Radius = m_scale;

	m_bomState->SetBoundingSphere(m_boundingSphere);

	float lastExplosionEnd = 0.0f;

	for (const auto& obj : m_explosionObjects) 
	{
		float endTime = obj.startTime + obj.duration;

		if (endTime > lastExplosionEnd) 
		{
			lastExplosionEnd = endTime;
		}
	}

	float totalElapsedTime = m_elapsedTime; // �Ⴆ�΃{���J�n����̗ݐώ���

	for (auto& obj : m_explosionObjects)
	{
		// �\���J�n�O
		if (totalElapsedTime < obj.startTime) {
			obj.active = false;
			continue;
		}

		// �\���I����
		if (totalElapsedTime > obj.startTime + obj.duration) {
			obj.active = false;
			continue;
		}

		// �\����
		obj.active = true;
	}

	if (m_elapsedTime >= lastExplosionEnd)
	{
		m_bomState->ChangeState(m_bomState->GetBomExist());
	}


}

void BomExplosion::PostUpdate()
{
	m_bomState->SetBomPresent(EXPLOSION);
	m_bomState->SetBomPresent(Exist);
	m_bomState->SetExist(false);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void BomExplosion::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();


	// �[�x�X�e���V���X�e�[�g�ݒ�
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	for (const auto& obj : m_explosionObjects)
	{
		if (!obj.active) continue;

		Vector3 drawPos = m_position + obj.offset;
		float drawScale = m_scale * obj.scaleMultiplier;

		Matrix worldExp = Matrix::CreateScale(Vector3(drawScale * 0.01f)) *
			Matrix::CreateRotationY(m_elapsedTime * 0.5f) *
			Matrix::CreateTranslation(drawPos);

		m_dithering->RenderObjects(context, states, projection, view, drawPos, worldExp, drawPos, m_explosionSphere.get());
	}

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
	//  �p�[�e�B�N���̕`��
	m_smork->Render(view, projection);
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void BomExplosion::Finalize()
{
	Sound::GetInstance().Finalize();
}

//---------------------------------------------------------
//�ǂ̔j������邩���Ȃ����̊m�F
//---------------------------------------------------------
void BomExplosion::CheckBreak()
{

	for (const auto& wall : m_wall)
	{
		// �o�E���f�B���O�X�t�B�A�ƕǂ�AABB���Փ˂��Ă��邩���`�F�b�N
		bool isHitWall = m_boundingSphere.Intersects(wall->GetBoundingBox());

		if (isHitWall)
		{
			Wall::WallType m_type = wall->GetWallType();

			if (m_type == Wall::WallType::Destructible && wall->IsVisible() == false)
			{
				wall->SetVisible(true); // ��\���ɂ���
				wall->SetExist(false);
			}

		}
	}
}

//---------------------------------------------------------
// �V�F�C�_�[�̓ǂݎ��
//---------------------------------------------------------
void BomExplosion::ShaderLoad(ID3D11Device* device)
{
	std::vector<uint8_t> psBlob = DX::ReadData(L"Resources/Shaders/ExplosionPS.cso");
	DX::ThrowIfFailed(device->CreatePixelShader(psBlob.data(), psBlob.size(), nullptr, m_ps.ReleaseAndGetAddressOf()));
	if (psBlob.empty()) {
		OutputDebugStringA("Failed to load vertex shader: PointLightPS.cso\n");
	}
}

//---------------------------------------------------------
// �萔�o�b�t�@�����
//---------------------------------------------------------
void BomExplosion::CreateConstanBuffer(ID3D11Device* device)
{
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
}
