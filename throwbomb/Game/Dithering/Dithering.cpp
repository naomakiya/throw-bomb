/*
    @file	Dithering.cpp
    @brief	ディザリングクラス
*/

#include "pch.h"
#include "Framework/DeviceResources.h"
#include "Dithering.h"
#include <Libraries/Microsoft/ReadData.h>

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Dithering::Dithering(ID3D11Device1* device)
    :
    m_objects{},
    m_constantBuffer{}
{
    // 定数バッファの設定情報を格納する構造体を初期化
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.ByteWidth = sizeof(cbLight);             // バッファのサイズ
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;             // 動的更新を許可
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;  // 定数バッファとして使用
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU からの書き換えを許可

    // 定数バッファを作成
    DX::ThrowIfFailed(
        device->CreateBuffer(&bufferDesc, nullptr, m_constantBuffer.ReleaseAndGetAddressOf())
    );

    // シェーダーを読み込む
    LoadShader(device);

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
Dithering::~Dithering()
{
}

//---------------------------------------------------------
// シェーダをロードする
//---------------------------------------------------------
void Dithering::LoadShader(ID3D11Device* device)
{
    // シェーダを読み込むためのblob
    std::vector<uint8_t> blob;

    blob = DX::ReadData(L"Resources/Shaders/VS_Dithering.cso");
    DX::ThrowIfFailed(
        device->CreateVertexShader(blob.data(), blob.size(), nullptr, m_vs.ReleaseAndGetAddressOf())
    );

    // ディザリング用のピクセルシェーダをロードする
    blob = DX::ReadData(L"Resources/Shaders/PS_Dithering.cso");
    DX::ThrowIfFailed(
        device->CreatePixelShader(blob.data(), blob.size(), nullptr, m_ps.ReleaseAndGetAddressOf())
    );
}

//---------------------------------------------------------
// オブジェクトを描画
//---------------------------------------------------------
void Dithering::RenderObjects(ID3D11DeviceContext* context,DirectX::DX11::CommonStates* states, DirectX::SimpleMath::Matrix projection, DirectX::SimpleMath::Matrix view,
  DirectX::SimpleMath::Vector3 eye,
    DirectX::SimpleMath::Matrix worlds,
    DirectX::SimpleMath::Vector3 position,
    DirectX::Model* model )
{
    // 動的メモリ確保
    ObjectInfo* obj = new ObjectInfo();  
    obj->world = worlds;
    obj->position = position;
    obj->model = model;

    // 定数バッファの更新
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    cbLight* cb = static_cast<cbLight*>(mappedResource.pData);
    cb->lightDirection = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    cb->viewMatrix = view.Transpose();
    cb->cameraPosition = eye;
    context->Unmap(m_constantBuffer.Get(), 0);

    // オブジェクトの描画
    obj->model->Draw(context, *states, obj->world, view, projection, false, [&]
        {
            ID3D11Buffer* cbuf[] = { m_constantBuffer.Get() };
            context->VSSetConstantBuffers(1, 1, cbuf);
            context->PSSetConstantBuffers(1, 1, cbuf);

            context->VSSetShader(m_vs.Get(), nullptr, 0);
            context->PSSetShader(m_ps.Get(), nullptr, 0);

            ID3D11SamplerState* sampler[] = { states->LinearWrap() };
            context->PSSetSamplers(0, 1, sampler);
        });

    // メモリの解放
    delete obj;
}
