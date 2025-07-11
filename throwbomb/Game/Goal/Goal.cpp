/*
  @file  Goal.cpp
  @brief �S�[���N���X
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Goal/Goal.h"
#include <Libraries/Microsoft/ReadData.h>

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
Goal::Goal(DirectX::SimpleMath::Vector3 pos)
	:
	m_commonResources{},
	m_model{},
	m_boundingSphere{},
	m_position{ pos },
	m_scale{0.025f},
	m_time{0.0f}
{
	//�o�E�f�B���O�X�t�B�A�̐ݒ�
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = 1.0f;
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
Goal::~Goal()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void Goal::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	
	//���f���̓ǂݎ��
	LoadModel(device);
	//�V�F�[�_�[�̓ǂݎ��
	LoadShader(device);
	//�萔�o�b�t�@�[�̍쐬
	CreateConstanBuffer(device);
}


//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void Goal::Render(ID3D11DeviceContext* context,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection)
{
	using namespace DirectX::SimpleMath;

	auto states = m_commonResources->GetCommonStates();

	// ���[���h�s����X�V����
	Matrix world = Matrix::CreateScale(m_scale);
	world *= Matrix::CreateTranslation(m_position);

	//�}�b�v���\�[�X�̍쐬
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// �萔�o�b�t�@���}�b�v����
	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//�萔�o�b�t�@�Ƀf�[�^��ݒ肷��
	PointLight* cb = static_cast<PointLight*>(mappedResource.pData);
	cb->position = Vector4{ m_position.x,m_position.y + 2.0f,m_position.z + 1.0f, 0.0f };
	cb->color = Vector4{ 2.0f,2.0f,0.0f,0.0f }; // 1���傫���������Ƃɂ�������������(���F�j
	cb->time = Vector4{ m_time,0.0f,0.0f,0.0f };

	// �}�b�v����������
	context->Unmap(m_constantBuffer.Get(), 0);

	//�`��
	m_model->Draw(context, *states, world, view, projection, false, [&]()
		{
			// �萔�o�b�t�@���w�肷��
			ID3D11Buffer* cbuf[] = { m_constantBuffer.Get() };
			context->PSSetConstantBuffers(1, 1, cbuf);

			// �V�F�[�_��ݒ肷��
			context->PSSetShader(m_ps.Get(), nullptr, 0);

			// �T���v���[�X�e�[�g���w�肷��
			ID3D11SamplerState* sampler[] = { states->LinearWrap() };
			context->PSSetSamplers(0, 1, sampler);
		});

	// �V�F�[�_�[������
	context->PSSetShader(nullptr, nullptr, 0);

}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void Goal::Finalize()
{
}

//---------------------------------------------------------
// ���f����ǂݎ��
//---------------------------------------------------------
void Goal::LoadModel(ID3D11Device* device)
{
	//// ���f����ǂݍ��ޏ���
	std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//���f�������[�h����
	m_model = DirectX::Model::CreateFromCMO(device, L"Resources/Models/TreasureChest.cmo", *fx);

	// �g�[���X���f���̃G�t�F�N�g��ݒ肷��
	m_model->UpdateEffects([&](DirectX::DX11::IEffect* effect)
		{
			auto basicEffect = dynamic_cast<DirectX::DX11::BasicEffect*>(effect);
			if (basicEffect){
				basicEffect->SetLightingEnabled(true);
				basicEffect->SetPerPixelLighting(true);
				basicEffect->SetTextureEnabled(true);
				basicEffect->SetVertexColorEnabled(false);
				basicEffect->SetFogEnabled(false);
			}
		}
	);
}

//---------------------------------------------------------
// �V�F�C�_�[�̓ǂݎ��
//---------------------------------------------------------
void Goal::LoadShader(ID3D11Device* device)
{
	std::vector<uint8_t> psBlob = DX::ReadData(L"Resources/Shaders/PointLightPS.cso");
	DX::ThrowIfFailed(device->CreatePixelShader(psBlob.data(), psBlob.size(), nullptr, m_ps.ReleaseAndGetAddressOf()));
	if (psBlob.empty()) {
		OutputDebugStringA("Failed to load vertex shader: PointLightPS.cso\n");
	}
}

//---------------------------------------------------------
// �萔�o�b�t�@�����
//---------------------------------------------------------
void Goal::CreateConstanBuffer(ID3D11Device* device)
{
	// �萔�o�b�t�@�p�̃o�b�t�@�I�u�W�F�N�g���쐬����
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(PointLight));	
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	DX::ThrowIfFailed(
		device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf())
	);
}

