/*
	@file	Floor.cpp
	@brief	���N���X
*/
#include "pch.h"
#include "Floor.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"

//---------------------------------------------------------
// �R���X�g���N�g
//---------------------------------------------------------
Floor::Floor()
	:
	m_commonResources{},
	m_position{DirectX::SimpleMath::Vector3(0.0f,0.0f,0.0f)}
{
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
Floor::~Floor()
{

}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void Floor::Initialize(CommonResources* resources)
{
	using namespace DirectX;

	assert(resources);

	// ���ʃ��\�[�X�̐ݒ�
	m_commonResources = resources;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// �G�t�F�N�g�̍쐬
	m_BatchEffect = std::make_unique<AlphaTestEffect>(device);
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_EQUAL);
	m_BatchEffect->SetReferenceAlpha(255);

	// ���̓��C�A�E�g����
	void const* shaderByteCode;
	size_t byteCodeLength;
	m_BatchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	device->CreateInputLayout(
		VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		shaderByteCode, byteCodeLength, m_InputLayout.GetAddressOf()
	);

	// �e�N�X�`���̃��[�h
	CreateWICTextureFromFile(
		device,
		ResourceManager::GetTexturePath("Floor").c_str(), 
		nullptr,
		m_Texture.GetAddressOf()
	);
	

	// �{�b�N�X���쐬�i�T�C�Y�F���A�[���A���ɔ��������j
	XMFLOAT3 size(FLOORWIDTH, 1.0f,FLOORDEPT); 

	// �����̂̍쐬
	m_floorPrimitive = GeometricPrimitive::CreateBox(context, size);

	// �o�E���f�B���O�{�b�N�X�̍쐬
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = size;

	// �[�x�X�e���V���X�e�[�g�ݒ�
	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.StencilEnable = true;
	desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace = desc.FrontFace;

	DX::ThrowIfFailed(
		device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf())
	);
}




//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void Floor::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	using namespace DirectX;

	auto states = m_commonResources->GetCommonStates();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ���[���h�s��i���̈ʒu�j
	SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::CreateTranslation(m_position);

	// �`�掞�̃G�t�F�N�g�ݒ�
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_GREATER);
	m_BatchEffect->SetReferenceAlpha(0);
	m_BatchEffect->SetWorld(worldMatrix);
	m_BatchEffect->SetView(view);
	m_BatchEffect->SetProjection(proj);
	m_BatchEffect->SetTexture(m_Texture.Get());
	m_BatchEffect->Apply(context);

	// �`��X�e�[�g�ݒ�
	
	// �[�x�X�e���V���X�e�[�g�ݒ�
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
	// �J�����O�ݒ�
	context->RSSetState(states->CullCounterClockwise()); 

	// �W�I���g���b�N�v���~�e�B�u�̕`��
	m_floorPrimitive->Draw(worldMatrix, view, proj, Colors::White, m_Texture.Get());
}


//---------------------------------------------------------
// ����
//---------------------------------------------------------
void Floor::Finalize()
{

}
