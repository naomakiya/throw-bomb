/*
    @file	Floor.h
    @brief	���N���X
*/
#pragma once
#ifndef FLOOR_DEFINED
#define FLOOR_DEFINED

#include <GeometricPrimitive.h>
#include <wrl/client.h>  

class  CommonResources;

class Floor
{
public:
    // �e�N�X�`���[�̍��W�Ƒ傫�����擾����֐�
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
public:
    // �R���X�g���N�^
    Floor(ID3D11Device1* device, DirectX::SimpleMath::Vector3 position, float width, float dept);
    //�f�X�g���N�^
    ~Floor();
    //������
    void Initialize(ID3D11Device1* device, CommonResources* resources);
    //�`��
    void Render(ID3D11DeviceContext1* context, DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);
    //�㏈��
    void Finalize();
private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //	���̓��C�A�E�g 
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
    //	�e�N�X�`���n���h�� 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
    //	�G�t�F�N�g 
    std::unique_ptr<DirectX::AlphaTestEffect> m_BatchEffect;
    // ���̌`��f�[�^
    std::unique_ptr<DirectX::GeometricPrimitive> m_floorPrimitive;
    // �[�x�X�e���V���X�e�[�g
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    //�o�E�f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
    // �e�N�X�`���[�̍��W
    DirectX::SimpleMath::Vector3 m_position;
    // �e�N�X�`���[�̕�
    float m_width;
    // �e�N�X�`���[�̉��s��
    float m_depth;
};

#endif  //FLOOR_DEFINED