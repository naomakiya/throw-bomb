#pragma once
#ifndef VERTEX_DEFINED
#define VERTEX_DEFINED
#include "SimpleMath.h"

// Vertexクラスを宣言する
class Vertex
{
public:
    // 最大頂点番号を取得する
    static int GetMaxNumber();
    // 最大頂点番号を再設定する
    static void ResetMaxNumber(int maxNumber);
    // 頂点番号を取得する
    const int GetVertexNumber() const;
    // 頂点番号を設定する
    void SetVertexNumber(const int& vertexNumber);
    // 頂点の位置を返す
    DirectX::SimpleMath::Vector2 GetPosition() const;
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector2& position);

public:
    // コンストラクタ
    Vertex(const DirectX::SimpleMath::Vector2& position);
    // デストラクタ
    ~Vertex();

private:
    // 最大頂点番号
    static int s_maxVertexNumber;
    // 頂点番号
    int m_vertexNumber;
    // 位置
    DirectX::SimpleMath::Vector2 m_position;
};

#endif  // VERTEX_DEFINED

