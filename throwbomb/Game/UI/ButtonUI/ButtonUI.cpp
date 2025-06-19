/*
	@file	ButtonUI.cpp
	@brief	ボタンに関するユーザーインターフェイスクラス
*/
#include "pch.h"
#include "Game/UI/ButtonUI/ButtonUI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include <cmath>
#include <cassert>
#include <Game/Screen.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
ButtonUI::ButtonUI()
    : 
    m_commonResources{},
    m_spriteBatch{},
    m_texCenter{},
    m_degree{ 0.0f },
    m_arrowTexture{},
    m_cameraUITexture{}
{
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
ButtonUI::~ButtonUI()
{
	
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void ButtonUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
{
    assert(resources);
    m_commonResources = resources;
    m_spriteBatch = spriteBatch;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    // 画像をロードする (矢印)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/arrow.png",
            nullptr,
            m_arrowTexture.ReleaseAndGetAddressOf()
        )
    );

    // 画像をロードする (ボタンA)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/BttunA.png",
            nullptr,
            m_ButtonUITextureA.ReleaseAndGetAddressOf()
        )
    );
    // 画像をロードする (ボタンD)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/BttunD.jpg",
            nullptr,
            m_ButtonUITextureD.ReleaseAndGetAddressOf()
        )
    );
    // 画像をロードする (カメラ)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/Camera.png",
            nullptr,
            m_cameraUITexture.ReleaseAndGetAddressOf()
        )
    );

    // テクスチャの中心位置を設定
    m_texCenter = Vector2(128.0f, 128.0f); // テクスチャの中心 (256x256 のテクスチャを想定)

}



//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void ButtonUI::Render()
{
    // ゲージの部分を描画
    m_spriteBatch->Draw(
        m_ButtonUITextureA.Get(),            // テクスチャ
        Vector2(25, 650),             // 位置
        nullptr,                        // ソース矩形
        Colors::White,                  // 色
        0,                      // 回転角度
        Vector2(0, 0),              // 回転の中心
        0.25f,                           // スケール
        SpriteEffects_None,             // スプライトエフェクト
        0.0f                            // レイヤー深度
    );
    //矢印テクスチャー
    m_spriteBatch->Draw(
        m_arrowTexture.Get(),            // テクスチャ
        Vector2(125, 675),             // 位置
        nullptr,                        // ソース矩形
        Colors::White,                  // 色
        0,                         // 回転角度
        Vector2(0, 0),              // 回転の中心
        0.05f,                           // スケール
        SpriteEffects_None,             // スプライトエフェクト
        0.0f                            // レイヤー深度
    );
    //カメラテクスチャー
    m_spriteBatch->Draw(
        m_cameraUITexture.Get(),            // テクスチャ
        Vector2(100, 550),             // 位置
        nullptr,                        // ソース矩形
        Colors::White,                  // 色
        0,                         // 回転角度
        Vector2(0, 0),              // 回転の中心
        0.15f,                           // スケール
        SpriteEffects_None,             // スプライトエフェクト
        0.0f                            // レイヤー深度
    );
    //Dボタンテクスチャー
    m_spriteBatch->Draw(
        m_ButtonUITextureD.Get(),            // テクスチャ
        Vector2(275, 650),             // 位置
        nullptr,                        // ソース矩形
        Colors::White,                  // 色
        0,                      // 回転角度
        Vector2(0, 0),              // 回転の中心
        0.25f,                           // スケール
        SpriteEffects_None,             // スプライトエフェクト
        0.0f                            // レイヤー深度
    );
    
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void ButtonUI::Finalize()
{
	// do nothing.
}

