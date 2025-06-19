/*
	@file	Floor.cpp
	@brief	���N���X
*/
#include "pch.h"
#include "Floor.h"
#include <WICTextureLoader.h> 
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
//#include <GeometricPrimitive.h>

//---------------------------------------------------------
// �R���X�g���N�g
//---------------------------------------------------------
Floor::Floor(ID3D11Device1* device,  DirectX::SimpleMath::Vector3 position, float width, float depth)
	:m_position(position),
	m_width(width),
	m_depth(depth)
{
	using namespace DirectX;

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
void Floor::Initialize(ID3D11Device1* device, CommonResources* resources)
{
	using namespace DirectX;

	assert(resources);
	// ���ʃ��\�[�X�̐ݒ�
	m_commonResources = resources;
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	// �e�N�X�`���̃��[�h
	CreateWICTextureFromFile(
		device,
		L"Resources/Textures/Floor.png", // ���̃e�N�X�`���t�@�C��
		nullptr,
		m_Texture.GetAddressOf()
	);
	
	// �{�b�N�X���쐬�i�T�C�Y�F���A�[���A���ɔ��������j
	XMFLOAT3 size(m_width, 1.0f, m_depth);  // �����͔��ɔ������Ƃ��Đݒ�
	//�����̂̍쐬
	m_floorPrimitive = GeometricPrimitive::CreateBox(context, size);
	//�@�o�E���f�B���O�{�b�N�X�̍쐬
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = size;

	// �[�x�X�e���V���X�e�[�g�ݒ�i���p�j
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
void Floor::Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	using namespace DirectX;
	auto states = m_commonResources->GetCommonStates();
	// ���[���h�s��i���̈ʒu�A�X�P�[�����O�j
	SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::CreateTranslation(m_position);

	// �`�掞�̃G�t�F�N�g�ݒ�
	m_BatchEffect->SetAlphaFunction(D3D11_COMPARISON_GREATER); // �s����
	m_BatchEffect->SetReferenceAlpha(0);
	m_BatchEffect->SetWorld(worldMatrix);
	m_BatchEffect->SetView(view);
	m_BatchEffect->SetProjection(proj);
	m_BatchEffect->SetTexture(m_Texture.Get());
	m_BatchEffect->Apply(context);

	// �`��X�e�[�g�ݒ�
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0); // �[�x�X�e���V���X�e�[�g�ݒ�
	context->RSSetState(states->CullCounterClockwise()); // �J�����O�ݒ�

	// �W�I���g���b�N�v���~�e�B�u�̕`��
	m_floorPrimitive->Draw(worldMatrix, view, proj, Colors::White, m_Texture.Get());
}


//---------------------------------------------------------
// ����
//---------------------------------------------------------
void Floor::Finalize()
{

}
