#include "pch.h"
#include "Vertex.h"

int Vertex::s_maxVertexNumber = 0;

// �A�N�Z�b�T:  �ő咸�_�ԍ����擾����
int Vertex::GetMaxNumber()
{
    return s_maxVertexNumber;
}

// �A�N�Z�b�T:  �ő咸�_�ԍ����Đݒ肷��
void Vertex::ResetMaxNumber(int maxNumber)
{
    s_maxVertexNumber = maxNumber;
}

// �A�N�Z�b�T:  ���_�ԍ����擾����
const int Vertex::GetVertexNumber() const
{
    // ���_�ԍ���Ԃ�
    return m_vertexNumber;
}

// �A�N�Z�b�T:  ���_�ԍ���ݒ肷��
void Vertex::SetVertexNumber(const int& vertexNumber)
{
    // ���_�ԍ���ݒ肷��
    m_vertexNumber = vertexNumber;
}

// �A�N�Z�b�T:  ���_�̈ʒu��Ԃ�
DirectX::SimpleMath::Vector2 Vertex::GetPosition() const
{
    // �ʒu��Ԃ�
    return m_position;
}

// �A�N�Z�b�T:  �ʒu��ݒ肷��
void Vertex::SetPosition(const DirectX::SimpleMath::Vector2& position)
{
    // �ʒu��ݒ肷��
    m_position = position;
}

// �R���X�g���N�^
Vertex::Vertex(const DirectX::SimpleMath::Vector2& position)
    :
    m_vertexNumber(0),        // ���_�ԍ�
    m_position{}                    // �ʒu
{
    // �J�E���g�A�b�v����
    m_vertexNumber = s_maxVertexNumber++;
    // �ʒu��ݒ肷��
    m_position = position;
}

// �f�X�g���N�^
Vertex::~Vertex()
{
}
