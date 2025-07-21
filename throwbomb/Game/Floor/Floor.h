/*
    @file	Floor.h
    @brief	���N���X
*/
#pragma once
#ifndef FLOOR_DEFINED
#define FLOOR_DEFINED
#include <GeometricPrimitive.h>

class  CommonResources;

class Floor
{
public:
    // ����
    static constexpr float FLOORWIDTH = 100.0f;
    // ����
    static constexpr float FLOORDEPT = 100.0f;

public:
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }

public:
    // �R���X�g���N�^
    Floor();
    //�f�X�g���N�^
    ~Floor();
    //������
    void Initialize(CommonResources* resources);
    //�`��
    void Render(const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& proj);
    //�㏈��
    void Finalize();

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //	�e�N�X�`���n���h�� 
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
    // ���̌`��f�[�^
    std::unique_ptr<DirectX::GeometricPrimitive> m_floorPrimitive;
    //�o�E�f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
    // �e�N�X�`���[�̍��W
    DirectX::SimpleMath::Vector3 m_position;
};

#endif  //FLOOR_DEFINED