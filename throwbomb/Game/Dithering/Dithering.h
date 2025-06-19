/*
    @file	Dithering.h
    @brief	�f�B�U�����O�N���X
*/
#pragma once
#ifndef DITHERING_DEFINED
#define DITHERING_DEFINED
// �O���錾
namespace DX
{
    class DeviceResources;
}

class Dithering
{
private:

    // �I�u�W�F�N�g����ێ�����\����
    struct ObjectInfo
    {
        DirectX::SimpleMath::Matrix world;      // ���[���h�ϊ��s��
        DirectX::SimpleMath::Vector3 position;  // �I�u�W�F�N�g�̃��[���h���W
        DirectX::Model* model = nullptr;                  // �`�悷��3D���f��
    };

    // ���C�g�̒萔�o�b�t�@�p�\����
    struct cbLight
    {
        DirectX::XMVECTOR lightDirection;		// ���C�g�̕���
        DirectX::XMMATRIX viewMatrix;			// �r���[�s��
        DirectX::XMVECTOR cameraPosition;		// �J�������W
    };
public:
// �R���X�g���N�^
    Dithering(ID3D11Device1* device);
    // �f�X�g���N�^
    ~Dithering();
    // �I�u�W�F�N�g��`�悷�郁�\�b�h
    void RenderObjects(ID3D11DeviceContext* context, DirectX::DX11::CommonStates* states, DirectX::SimpleMath::Matrix projection, DirectX::SimpleMath::Matrix view,
        DirectX::SimpleMath::Vector3 eye,
        DirectX::SimpleMath::Matrix worlds,
        DirectX::SimpleMath::Vector3 position,
        DirectX::Model* model);
private:
    // �V�F�[�_�����[�h����
    void LoadShader(ID3D11Device* device);
    // �I�u�W�F�N�g�̃��X�g
    std::vector<ObjectInfo> m_objects;
    //���_�V�F�C�_�[
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    //�s�N�Z���V�F�C�_�[
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    // �萔�o�b�t�@�p�̃o�b�t�@�I�u�W�F�N�g
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

    
};
#endif //DITHERING_DEFINED


