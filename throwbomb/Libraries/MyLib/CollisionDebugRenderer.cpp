/*
	@file	CollisionDebugRenderer.cpp
	@brief	�R���W�����̏�����������N���X
*/
#include "pch.h"
#include "CollisionDebugRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;

CollisionDebugRenderer::CollisionDebugRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
    : 
    m_device(device), 
    m_context(context)
{
    assert(device && context);


    m_primitiveBatch = std::make_unique<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_context);
    m_basicEffect = std::make_unique<DirectX::BasicEffect>(device);
    m_basicEffect->SetVertexColorEnabled(true);

    // �C���v�b�g���C�A�E�g���쐬����
    void const* shaderByteCode;
    size_t byteCodeLength;
    m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(
        device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
            DirectX::VertexPositionColor::InputElementCount,
            shaderByteCode, byteCodeLength,
            m_inputLayout.ReleaseAndGetAddressOf())
    );

    // �[�x�e�X�g�𖳌��ɂ��邽�߂̃X�e���V���X�e�[�g���쐬
    ID3D11DepthStencilState* depthStencilState = nullptr;
    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.DepthEnable = false;  // �[�x�e�X�g�𖳌���
    desc.StencilEnable = false; // �X�e���V���e�X�g��������

   
    // �쐬�����X�e�[�g��K�p
    context->OMSetDepthStencilState(depthStencilState, 1);
    if (depthStencilState)
    {
        depthStencilState->Release();
    }
}

CollisionDebugRenderer::~CollisionDebugRenderer()
{
}

void CollisionDebugRenderer::RenderBoundingBox(
    const DirectX::BoundingBox& boundingBox,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection,
    const DirectX::SimpleMath::Vector4& color)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;


    m_basicEffect->SetView(view);
    m_basicEffect->SetProjection(projection);
    m_basicEffect->SetWorld(Matrix::Identity);
    m_basicEffect->Apply(m_context);
    m_context->IASetInputLayout(m_inputLayout.Get());

    m_primitiveBatch->Begin();

    Vector3 corners[8];
    boundingBox.GetCorners(corners);

    // ���C���[�t���[���Ƃ��ăR�[�i�[��`��
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[0], color), VertexPositionColor(corners[1], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[1], color), VertexPositionColor(corners[2], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[2], color), VertexPositionColor(corners[3], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[3], color), VertexPositionColor(corners[0], color));

    m_primitiveBatch->DrawLine(VertexPositionColor(corners[4], color), VertexPositionColor(corners[5], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[5], color), VertexPositionColor(corners[6], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[6], color), VertexPositionColor(corners[7], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[7], color), VertexPositionColor(corners[4], color));

    m_primitiveBatch->DrawLine(VertexPositionColor(corners[0], color), VertexPositionColor(corners[4], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[1], color), VertexPositionColor(corners[5], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[2], color), VertexPositionColor(corners[6], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[3], color), VertexPositionColor(corners[7], color));

    m_primitiveBatch->End();
}

void CollisionDebugRenderer::RenderBoundingSphere(const DirectX::BoundingSphere& boundingSphere, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection, const DirectX::SimpleMath::Vector4& color)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    m_basicEffect->SetView(view);
    m_basicEffect->SetProjection(projection);
    m_basicEffect->SetWorld(Matrix::Identity);
    m_basicEffect->Apply(m_context);
    m_context->IASetInputLayout(m_inputLayout.Get());

    // �v���~�e�B�u�o�b�`�̊J�n
    m_primitiveBatch->Begin();

    // �X�t�B�A��`�悷�邽�߂̃|�C���g���v�Z���܂�
    const int slices = 24; // �X�t�B�A�̕�����
    const int stacks = 12; // �X�^�b�N��

    // �X�t�B�A�̎��̊p�x
    float phiStep = XM_PI / stacks;
    float thetaStep = 2.0f * XM_PI / slices;

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            // ���݂̓_�̈ʒu���v�Z
            float theta = j * thetaStep; // �ܓx
            float phi = (i + 1) * phiStep; // �o�x

            // �_�����ʍ��W���璼�����W�ɕϊ�
            Vector3 p1 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            theta = (j + 1) * thetaStep;
            Vector3 p2 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            // ����`��
            m_primitiveBatch->DrawLine(VertexPositionColor(p1, color), VertexPositionColor(p2, color));
        }

        // �o�x��ς������̐����`��
        for (int j = 0; j < slices; ++j)
        {
            // �o�x�̓_���v�Z
            float theta = j * thetaStep; // �ܓx
            float phi = (i)*phiStep; // �o�x

            // �_�����ʍ��W���璼�����W�ɕϊ�
            Vector3 p1 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            phi = (i + 1) * phiStep; // ���̃X�^�b�N
            Vector3 p2 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            // ����`��
            m_primitiveBatch->DrawLine(VertexPositionColor(p1, color), VertexPositionColor(p2, color));
        }
    }

    // �v���~�e�B�u�o�b�`�̏I��
    m_primitiveBatch->End();
}

void CollisionDebugRenderer::RenderBoundingOrientedBox(
    const DirectX::BoundingOrientedBox& orientedBox,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection,
    const DirectX::SimpleMath::Vector4& color)
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    // �r���[�E�v���W�F�N�V�����s���ݒ�
    m_basicEffect->SetView(view);
    m_basicEffect->SetProjection(projection);
    m_basicEffect->SetWorld(Matrix::Identity);
    m_basicEffect->Apply(m_context);
    m_context->IASetInputLayout(m_inputLayout.Get());

    m_primitiveBatch->Begin();

    // �o�E���f�B���O�{�b�N�X�̃R�[�i�[���擾
    Vector3 corners[8];
    orientedBox.GetCorners(corners);

    // ���C���[�t���[���Ƃ��ăR�[�i�[��`��
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[0], color), VertexPositionColor(corners[1], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[1], color), VertexPositionColor(corners[2], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[2], color), VertexPositionColor(corners[3], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[3], color), VertexPositionColor(corners[0], color));

    m_primitiveBatch->DrawLine(VertexPositionColor(corners[4], color), VertexPositionColor(corners[5], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[5], color), VertexPositionColor(corners[6], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[6], color), VertexPositionColor(corners[7], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[7], color), VertexPositionColor(corners[4], color));

    m_primitiveBatch->DrawLine(VertexPositionColor(corners[0], color), VertexPositionColor(corners[4], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[1], color), VertexPositionColor(corners[5], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[2], color), VertexPositionColor(corners[6], color));
    m_primitiveBatch->DrawLine(VertexPositionColor(corners[3], color), VertexPositionColor(corners[7], color));

    m_primitiveBatch->End();
}
