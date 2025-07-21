/*
	@file	HollowHartUI.cpp
	@brief	空のハートクラス
*/

#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/UI/Hart/HollowHartUI.h"
#include <Game/Screen.h>


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
HollowHartUI::HollowHartUI()
	:
	m_commonResources{},
	m_spriteBatch{},
	m_texture{},
    m_texSize{0.0f}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
HollowHartUI::~HollowHartUI()
{
	
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void HollowHartUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
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
            ResourceManager::GetTexturePath("HartEmpty").c_str(),
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
void HollowHartUI::Render(const int index)
{
    // 空ハートの初期位置の設定
    float startPos = Screen::CENTER_X - m_texSize.x;
    // 空ハート描画位置を決める
    DirectX::SimpleMath::Vector2 pos(startPos + m_texSize.x *(2 - index), Screen::TOP );
    // ハートを描画する
    m_spriteBatch->Draw(m_texture.Get(), pos);
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void HollowHartUI::Finalize()
{
	
}
