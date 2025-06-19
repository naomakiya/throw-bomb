#pragma once
#ifndef VERTEX_DEFINED
#define VERTEX_DEFINED
#include "SimpleMath.h"

// Vertex�N���X��錾����
class Vertex
{
public:
    // �ő咸�_�ԍ����擾����
    static int GetMaxNumber();
    // �ő咸�_�ԍ����Đݒ肷��
    static void ResetMaxNumber(int maxNumber);
    // ���_�ԍ����擾����
    const int GetVertexNumber() const;
    // ���_�ԍ���ݒ肷��
    void SetVertexNumber(const int& vertexNumber);
    // ���_�̈ʒu��Ԃ�
    DirectX::SimpleMath::Vector2 GetPosition() const;
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector2& position);

public:
    // �R���X�g���N�^
    Vertex(const DirectX::SimpleMath::Vector2& position);
    // �f�X�g���N�^
    ~Vertex();

private:
    // �ő咸�_�ԍ�
    static int s_maxVertexNumber;
    // ���_�ԍ�
    int m_vertexNumber;
    // �ʒu
    DirectX::SimpleMath::Vector2 m_position;
};

#endif  // VERTEX_DEFINED

