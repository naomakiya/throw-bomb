/*
    @file	Dithering.cpp
    @brief	�f�B�U�����O�N���X
*/

#include "pch.h"
#include "Framework/DeviceResources.h"
#include "Dithering.h"
#include <Libraries/Microsoft/ReadData.h>

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
Dithering::Dithering(ID3D11Device1* device)
    :
    m_objects{},
    m_constantBuffer{}
{
    // �萔�o�b�t�@�̐ݒ�����i�[����\���̂�������
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(cbLight);             // �o�b�t�@�̃T�C�Y
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;             // ���I�X�V������
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;  // �萔�o�b�t�@�Ƃ��Ďg�p
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU ����̏�������������

    // �萔�o�b�t�@���쐬
    DX::ThrowIfFailed(
        device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf())
    );

    // �V�F�[�_�[��ǂݍ���
    LoadShader(device);

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
Dithering::~Dithering()
{
}

//---------------------------------------------------------
// �V�F�[�_�����[�h����
//---------------------------------------------------------
void Dithering::LoadShader(ID3D11Device* device)
{
    // �V�F�[�_��ǂݍ��ނ��߂�blob
    std::vector<uint8_t> blob;

    blob = DX::ReadData(L"Resources/Shaders/VS_Dithering.cso");
    DX::ThrowIfFailed(
        device->CreateVertexShader(blob.data(), blob.size(), nullptr, m_vs.ReleaseAndGetAddressOf())
    );

    // �f�B�U�����O�p�̃s�N�Z���V�F�[�_�����[�h����
    blob = DX::ReadData(L"Resources/Shaders/PS_Dithering.cso");
    DX::ThrowIfFailed(
        device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
    );
}

//---------------------------------------------------------
// �I�u�W�F�N�g��`��
//---------------------------------------------------------
void Dithering::RenderObjects(ID3D11DeviceContext* context,DirectX::DX11::CommonStates* states, DirectX::SimpleMath::Matrix projection, DirectX::SimpleMath::Matrix view,
  DirectX::SimpleMath::Vector3 eye,
    DirectX::SimpleMath::Matrix worlds,
    DirectX::SimpleMath::Vector3 position,
    DirectX::Model* model )
{
    // ���I�������m��
    ObjectInfo* obj = new ObjectInfo();  
    obj->world = worlds;
    obj->position = position;
    obj->model = model;

    // �萔�o�b�t�@�̍X�V
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    cbLight* cb = static_cast<cbLight*>(mappedResource.pData);
    cb->lightDirection = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    cb->viewMatrix = view.Transpose();
    cb->cameraPosition = eye;
    context->Unmap(m_constantBuffer.Get(), 0);

    // �I�u�W�F�N�g�̕`��
    obj->model->Draw(context, *states, obj->world, view, projection, false, [&]
        {
            ID3D11Buffer* cbuf[] = { m_constantBuffer.Get() };
            context->VSSetConstantBuffers(1, 1, cbuf);
            context->PSSetConstantBuffers(1, 1, cbuf);

            context->VSSetShader(m_vs.Get(), nullptr, 0);
            context->PSSetShader(m_ps.Get(), nullptr, 0);

            ID3D11SamplerState* sampler[] = { states->LinearWrap() };
            context->PSSetSamplers(0, 1, sampler);
        });

    // �������̉��
    delete obj;
}
