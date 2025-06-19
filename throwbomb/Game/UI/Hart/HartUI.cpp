/*
	@file	HartUI.cpp
	@brief	ハートクラス
*/
#include "pch.h"
#include "Game/UI/Hart/HartUI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include <Game/Screen.h>



//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
HartUI::HartUI()
	:
	m_commonResources{},
	m_spriteBatch{},
	m_texture{},
	m_texSize{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
HartUI::~HartUI()
{
	
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void HartUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
{
    using namespace DirectX;
    
	assert(resources);
    assert(spriteBatch);
	m_commonResources = resources;
	m_spriteBatch = spriteBatch;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    // 画像をロードする
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/hart.png",
            nullptr,
            m_texture.ReleaseAndGetAddressOf()
        )
    );

    // 一時的な変数の宣言
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
    D3D11_TEXTURE2D_DESC desc;

    // テクスチャの情報を取得する
    m_texture->GetResource(resource.GetAddressOf());
    resource.As(&tex2D);
    tex2D->GetDesc(&desc);

    // テクスチャサイズを取得し、float型に変換する
    m_texSize.x = static_cast<float>(desc.Width);
    m_texSize.y = static_cast<float>(desc.Height);

}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void HartUI::Render(const int index)
{
    //ハートの初期位置の設定
    float startPos = Screen::CENTER_X - m_texSize.x;

    //ハートの描画位置を決める
    DirectX::SimpleMath::Vector2 pos(startPos + m_texSize.x  * index, Screen::TOP );

    // ハートを描画する
    m_spriteBatch->Draw(m_texture.Get(), pos);
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void HartUI::Finalize()
{
	
}
