/*
	@file	BomUI.cpp
	@brief	�{��UI�N���X
*/
#include "pch.h"
#include "Game/UI/BomUI/BomUI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include <Libraries/MyLib/BinaryFile.h>
#include "Game/UI/UI.h"


//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
BomUI::BomUI()
	:
	m_commonResources{},
	m_spriteBatch{},
	m_bomTexture{},
	m_texCenter{},
	m_inputLayout{},
	m_vertexShader{},
	m_pixelShader{},
	m_geometryShader{},
	m_CBuffer{},
	m_time{99},
	m_sin{}

{
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
BomUI::~BomUI()
{
	
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void BomUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
{
	assert(resources);
    assert(spriteBatch);
	m_commonResources = resources;
	m_spriteBatch = spriteBatch;

    //	�V�F�[�_�[�̍쐬
    CreateShader();
    //�摜�̓ǂݍ���
    LoadTexture(L"Resources/Textures/Boms.png");
    m_batch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>>(m_commonResources->GetDeviceResources()->GetD3DDeviceContext());
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void BomUI::Render(float elapsedTime)
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	//���Ԍo��
	m_time -= elapsedTime;
	
	//	���_���(�|���S���̂S���_�̍��W���j
	DirectX::VertexPositionColorTexture vertex[4] =
	{
		DirectX::VertexPositionColorTexture(DirectX::SimpleMath::Vector3(0.0f,  -0.75f, 0.0f),
		DirectX::SimpleMath::Vector4::One, DirectX::SimpleMath::Vector2(0.0f, 0.0f)),
	};

	ConstBuffer cbuff;
	//	�V�F�[�_�[�ɓn���ǉ��̃o�b�t�@���쐬����B
	//�c��30�b�ɂȂ�����
	if (m_time < 30.0f)
	{
		//�傫���̒���
		m_sin = sin((m_time) + 25) / 10 + 0.1f;

		cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld *= DirectX::SimpleMath::Matrix::CreateScale(m_sin + 0.15f, m_sin + 0.275f, 1.0f);
		cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
		cbuff.time = DirectX::SimpleMath::Vector4(m_time, 1, 1, 1);
	}
	else
	{
		
		cbuff.matView = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matProj = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld = DirectX::SimpleMath::Matrix::Identity;
		cbuff.matWorld *= DirectX::SimpleMath::Matrix::CreateScale(0.15f, 0.275f, 1.0f);
		cbuff.Diffuse = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
		cbuff.time = DirectX::SimpleMath::Vector4(m_time, 1, 1, 1);
	}
	
	//	�V�F�[�_�[�Ƀo�b�t�@��n��
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	//	�󂯓n���p�o�b�t�@�̓��e�X�V(ConstBuffer����ID3D11Buffer�ւ̕ϊ��j
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	�摜�p�T���v���[�̓o�^
	ID3D11SamplerState* sampler[1] = { states->LinearWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	�������`��w��
	ID3D11BlendState* blendstate = states->NonPremultiplied();

	//	�������菈��
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	�[�x�o�b�t�@�ɏ������ݎQ�Ƃ���
	context->OMSetDepthStencilState(states->DepthDefault(), 0);

	//	�J�����O�͍�����
	context->RSSetState(states->CullNone());

	//	�V�F�[�_���Z�b�g����
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	context->PSSetShaderResources(0, 1, m_bomTexture.GetAddressOf());
	

	//	�C���v�b�g���C�A�E�g�̓o�^
	context->IASetInputLayout(m_inputLayout.Get());

	//	�|���S����`��	//�{����`�悷��
	m_batch->Begin();
	m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, &vertex[0], 1);
	m_batch->End();

	

	//	�V�F�[�_�̓o�^���������Ă���
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
    
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void BomUI::Finalize()
{
	// do nothing.
}

//---------------------------------------------------------
// Shader�쐬������������
//---------------------------------------------------------
void BomUI::CreateShader()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	//	�R���p�C�����ꂽ�V�F�[�_�t�@�C����ǂݍ���
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleGS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticlePS.cso");

	//	�C���v�b�g���C�A�E�g�̍쐬
	device->CreateInputLayout(&UI::INPUT_LAYOUT[0],
		static_cast<UINT>(UI::INPUT_LAYOUT.size()),
		VSData.GetData(), VSData.GetSize(),
		m_inputLayout.GetAddressOf());

	//	���_�V�F�[�_�쐬
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{//	�G���[
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	//	�W�I���g���V�F�[�_�쐬
	if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{//	�G���[
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}
	//	�s�N�Z���V�F�[�_�쐬
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{//	�G���[
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	//	�V�F�[�_�[�Ƀf�[�^��n�����߂̃R���X�^���g�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}

//---------------------------------------------------------
///	�e�N�X�`�����\�[�X�ǂݍ���
//---------------------------------------------------------
void BomUI::LoadTexture(const wchar_t* path)
{
    DirectX::CreateWICTextureFromFile(m_commonResources->GetDeviceResources()->GetD3DDevice(), path, nullptr, m_bomTexture.ReleaseAndGetAddressOf());
    
}