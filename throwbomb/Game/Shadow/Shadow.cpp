/*
	@file	Shadow.cpp
	@brief	�ۉe��\������N���X
*/
#include "pch.h"
#include "Shadow.h"

using namespace DirectX;

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
Shadow::Shadow()
	:
	m_texture{},
	m_primitiveBatch{},
	m_basicEffect{},
	m_inputLayout{},
	m_depthStencilState{},
	m_alphaBlendState{}
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void Shadow::Initialize(
	ID3D11Device* device,
	ID3D11DeviceContext* context
)
{
	assert(device);
	assert(context);

	// �x�[�V�b�N�G�t�F�N�g��ݒ肷��
	m_basicEffect = std::make_unique<BasicEffect>(device);
	m_basicEffect->SetVertexColorEnabled(false);
	m_basicEffect->SetTextureEnabled(true);
	m_basicEffect->SetLightingEnabled(false);

	// ���̓��C�A�E�g��ݒ肷��
	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionTexture>(
			device,
			m_basicEffect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()
		)
	);

	// �v���~�e�B�u�o�b�`�𐶐�����
	m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

	// �摜��ǂݍ���
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(
			device,
			L"Resources/Textures/Shadow.png",
			nullptr,
			m_texture.ReleaseAndGetAddressOf()
		)
	);

	// �[�x�X�e���V���o�b�t�@��ݒ肷��
	D3D11_DEPTH_STENCIL_DESC desc{};
	desc.DepthEnable = true;                                    // �[�x�e�X�g���s��
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;			// �[�x�o�b�t�@���X�V����
	desc.DepthFunc = D3D11_COMPARISON_LESS;                     // �[�x�l��菬�����ꍇ�X�V

	desc.StencilEnable = false;                                 // �X�e���V���e�X�g���s��
	desc.StencilReadMask = D3D11_DEPTH_WRITE_MASK_ZERO;			// 0xff
	desc.StencilWriteMask = D3D11_COMPARISON_LESS_EQUAL;		// 0xff

	// �\��
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;        // �Q�ƒl�ƃX�e���V���l�����l�Ȃ�
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;       // OK �������Ȃ�
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;       // NG �������Ȃ�
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // NG �������Ȃ�

	// ���ʂ������ݒ�
	desc.BackFace = desc.FrontFace;

	// �[�x�X�e���V���X�e�[�g���쐬����
	DX::ThrowIfFailed(device->CreateDepthStencilState(&desc, m_depthStencilState.ReleaseAndGetAddressOf()));

	// �A���t�@�u�����h�X�e�[�g���쐬���镔���������ɒǉ�
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	DX::ThrowIfFailed(device->CreateBlendState(&blendDesc, m_alphaBlendState.ReleaseAndGetAddressOf()));
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void Shadow::Render(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection,
	const DirectX::SimpleMath::Vector3& position,
	float radius
)
{
	using namespace DirectX::SimpleMath;

	assert(context);
	assert(states);

	// �[�x�X�e���V���X�e�[�g��K�p����
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);	// �Q�ƒl�F1

	// �J�����O��ݒ肷��
	context->RSSetState(states->CullCounterClockwise());

	// �e�N�X�`���T���v����K�p����
	ID3D11SamplerState* sampler = states->LinearClamp();
	context->PSSetSamplers(0, 1, &sampler);

	// ���̓��C�A�E�g��K�p����
	context->IASetInputLayout(m_inputLayout.Get());

	// �G�t�F�N�g��ݒ肵�K�p����
	m_basicEffect->SetWorld(Matrix::Identity);
	m_basicEffect->SetView(view);
	m_basicEffect->SetProjection(projection);
	m_basicEffect->SetTexture(m_texture.Get());
	m_basicEffect->Apply(context);

	// �e�|���S���̒��_�����`����
	VertexPositionTexture vertices[] = {
		VertexPositionTexture(Vector3::Zero, Vector2(0.0f, 0.0f)),
		VertexPositionTexture(Vector3::Zero, Vector2(1.0f, 0.0f)),
		VertexPositionTexture(Vector3::Zero, Vector2(0.0f, 1.0f)),
		VertexPositionTexture(Vector3::Zero, Vector2(1.0f, 1.0f)),
	};

	// �e�|���S���̃C���f�b�N�X�����`����FDrawIndexed�ŕ`�悷��Ƃ��ɕK�v
	uint16_t indexes[] = { 0, 1, 2, 1, 3, 2 };

	// �e�|���S���̒��_����ݒ肷��iY���W��������������j
	float shadowOffset = 0.1f;  // �����グ��
	float scaleFactor = 1.0f;   // �e�̑傫����2�{�ɂ���X�P�[���W��
	// �e�|���S���̈ʒu��ݒ�
	vertices[0].position = Vector3(-radius * scaleFactor, shadowOffset, -radius * scaleFactor) + position;
	vertices[1].position = Vector3(radius * scaleFactor, shadowOffset, -radius * scaleFactor) + position;
	vertices[2].position = Vector3(-radius * scaleFactor, shadowOffset, radius * scaleFactor) + position;
	vertices[3].position = Vector3(radius * scaleFactor, shadowOffset, radius * scaleFactor) + position;

	// �A���t�@�u�����h�X�e�[�g��K�p���镔��
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(m_alphaBlendState.Get(), blendFactor, 0xffffffff);

	// �e�|���S����`�悷��
	m_primitiveBatch->Begin();
	m_primitiveBatch->DrawIndexed(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		indexes,
		_countof(indexes),
		vertices,
		_countof(vertices)
	);
	m_primitiveBatch->End();

	// �u�����h�X�e�[�g�����Z�b�g
	context->OMSetBlendState(nullptr, nullptr, 0xffffffff);


	// �u�����h�X�e�[�g�����Z�b�g
	context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}
