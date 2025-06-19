/*
	@file	CollisionDebugRenderer.cpp
	@brief	コリジョンの情報を可視化するクラス
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

    // インプットレイアウトを作成する
    void const* shaderByteCode;
    size_t byteCodeLength;
    m_basicEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

    DX::ThrowIfFailed(
        device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
            DirectX::VertexPositionColor::InputElementCount,
            shaderByteCode, byteCodeLength,
            m_inputLayout.ReleaseAndGetAddressOf())
    );

    // 深度テストを無効にするためのステンシルステートを作成
    ID3D11DepthStencilState* depthStencilState = nullptr;
    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.DepthEnable = false;  // 深度テストを無効化
    desc.StencilEnable = false; // ステンシルテストも無効化

   
    // 作成したステートを適用
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

    // ワイヤーフレームとしてコーナーを描画
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

    // プリミティブバッチの開始
    m_primitiveBatch->Begin();

    // スフィアを描画するためのポイントを計算します
    const int slices = 24; // スフィアの分割数
    const int stacks = 12; // スタック数

    // スフィアの周の角度
    float phiStep = XM_PI / stacks;
    float thetaStep = 2.0f * XM_PI / slices;

    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            // 現在の点の位置を計算
            float theta = j * thetaStep; // 緯度
            float phi = (i + 1) * phiStep; // 経度

            // 点を球面座標から直交座標に変換
            Vector3 p1 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            theta = (j + 1) * thetaStep;
            Vector3 p2 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            // 線を描画
            m_primitiveBatch->DrawLine(VertexPositionColor(p1, color), VertexPositionColor(p2, color));
        }

        // 経度を変えた時の線も描画
        for (int j = 0; j < slices; ++j)
        {
            // 経度の点を計算
            float theta = j * thetaStep; // 緯度
            float phi = (i)*phiStep; // 経度

            // 点を球面座標から直交座標に変換
            Vector3 p1 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            phi = (i + 1) * phiStep; // 次のスタック
            Vector3 p2 = boundingSphere.Center + Vector3(
                boundingSphere.Radius * sinf(phi) * cosf(theta),
                boundingSphere.Radius * cosf(phi),
                boundingSphere.Radius * sinf(phi) * sinf(theta));

            // 線を描画
            m_primitiveBatch->DrawLine(VertexPositionColor(p1, color), VertexPositionColor(p2, color));
        }
    }

    // プリミティブバッチの終了
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

    // ビュー・プロジェクション行列を設定
    m_basicEffect->SetView(view);
    m_basicEffect->SetProjection(projection);
    m_basicEffect->SetWorld(Matrix::Identity);
    m_basicEffect->Apply(m_context);
    m_context->IASetInputLayout(m_inputLayout.Get());

    m_primitiveBatch->Begin();

    // バウンディングボックスのコーナーを取得
    Vector3 corners[8];
    orientedBox.GetCorners(corners);

    // ワイヤーフレームとしてコーナーを描画
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
