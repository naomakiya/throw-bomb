/*
	@file	CollisionDebugRenderer.h
	@brief	�R���W�����̏�����������N���X
*/
#pragma once


 class CollisionDebugRenderer
{
        public:
        CollisionDebugRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
        ~CollisionDebugRenderer();

        
        // �R���W�����iBoundingBox�j�̕`�惁�\�b�h
        void RenderBoundingBox(
            const DirectX::BoundingBox& boundingBox,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& projection,
            const DirectX::SimpleMath::Vector4& color = DirectX::SimpleMath::Vector4(DirectX::Colors::Red));
        // �R���W�����iBoundingSphere�j�̕`�惁�\�b�h
        void RenderBoundingSphere(
            const DirectX::BoundingSphere& boundingSphere,
            const DirectX::SimpleMath::Matrix& view,
            const DirectX::SimpleMath::Matrix& projection,
            const DirectX::SimpleMath::Vector4& color = DirectX::SimpleMath::Vector4(DirectX::Colors::Red));
        // �R���W�����iBoundingOrientedBox�j�̕`�惁�\�b�h
        void RenderBoundingOrientedBox(const DirectX::BoundingOrientedBox& orientedBox, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection, const DirectX::SimpleMath::Vector4& color);

        private:
        // �����o�ϐ�
        std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
        std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
        ID3D11Device* m_device;
        ID3D11DeviceContext* m_context;
 };
