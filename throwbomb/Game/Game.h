//
// Game.h
//

#pragma once

#include "Framework/DeviceResources.h"
#include "Framework/StepTimer.h"

#include"Framework/CommonResources.h"
#include"Interface/IScene.h"
#include "Game/Scene/SceneManager.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/InputManager.h"

// 基本的なゲーム実装で、D3D11デバイスと
// ゲームループを提供する
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // 初期化と管理
    void Initialize(HWND window, int width, int height);

    // 基本的な試合のループ
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // メッセージ
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // プロパティ
    void GetDefaultSize(int& width, int& height) const noexcept;

    void SetFullscreenState(BOOL value)
    {
        m_fullscreen = value;
        m_deviceResources->GetSwapChain()->SetFullscreenState(m_fullscreen, nullptr);
        if (value) m_deviceResources->CreateWindowSizeDependentResources();
    }
private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // デバイスのリソース
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // ウィンドウハンドル
    HWND                                    m_hWnd;

    //レンダリングループタイマー
    DX::StepTimer                           m_timer;

    // コモンステート
    std::unique_ptr<DirectX::CommonStates>  m_commonStates;

    // コモンリソース
    std::unique_ptr<CommonResources>        m_commonResources;

    // デバッグストリング
    std::unique_ptr<mylib::DebugString>     m_debugString;

    // 入力マネージャ
    std::unique_ptr<mylib::InputManager>    m_inputManager;

    // シーンマネージャ
    std::unique_ptr<SceneManager>           m_sceneManager;

    //フルスクリーンにするかしないか
    BOOL                                    m_fullscreen;

};
