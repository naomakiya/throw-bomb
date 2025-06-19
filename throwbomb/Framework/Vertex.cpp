#include "pch.h"
#include "Vertex.h"

int Vertex::s_maxVertexNumber = 0;

// アクセッサ:  最大頂点番号を取得する
int Vertex::GetMaxNumber()
{
    return s_maxVertexNumber;
}

// アクセッサ:  最大頂点番号を再設定する
void Vertex::ResetMaxNumber(int maxNumber)
{
    s_maxVertexNumber = maxNumber;
}

// アクセッサ:  頂点番号を取得する
const int Vertex::GetVertexNumber() const
{
    // 頂点番号を返す
    return m_vertexNumber;
}

// アクセッサ:  頂点番号を設定する
void Vertex::SetVertexNumber(const int& vertexNumber)
{
    // 頂点番号を設定する
    m_vertexNumber = vertexNumber;
}

// アクセッサ:  頂点の位置を返す
DirectX::SimpleMath::Vector2 Vertex::GetPosition() const
{
    // 位置を返す
    return m_position;
}

// アクセッサ:  位置を設定する
void Vertex::SetPosition(const DirectX::SimpleMath::Vector2& position)
{
    // 位置を設定する
    m_position = position;
}

// コンストラクタ
Vertex::Vertex(const DirectX::SimpleMath::Vector2& position)
    :
    m_vertexNumber(0),        // 頂点番号
    m_position{}                    // 位置
{
    // カウントアップする
    m_vertexNumber = s_maxVertexNumber++;
    // 位置を設定する
    m_position = position;
}

// デストラクタ
Vertex::~Vertex()
{
}
