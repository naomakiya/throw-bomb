//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "Game/Scene/SceneManager.h"


extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
    : 
    m_deviceResources{},
    m_timer{},
    m_commonStates{},
    m_commonResources{},
    m_debugString{},
    m_inputManager{},
    m_sceneManager{},	
    m_fullscreen(false)
{
    // デバイスリソースを取得する
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // デバイスの通知先を設定する
    //m_deviceResources->RegisterDeviceNotify(this);
}

// 実行に必要なDirect3Dリソースを初期化する。
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

     //デバイスとコンテキストを取得する
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // 入力マネージャを作成する
    m_inputManager = std::make_unique<mylib::InputManager>(window);

    // コモンステートを作成する
    m_commonStates = std::make_unique<CommonStates>(device);

    // デバッグ文字列を作成する
    m_debugString = std::make_unique<mylib::DebugString>(
        device,
        context,
        L"Resources/Fonts/SegoeUI_18.spritefont"
    );

    // 共通リソースを作成する
    m_commonResources = std::make_unique<CommonResources>();

    // 画面サイズ
    int screenWidth, screenHeight;

    // 画面サイズを取得する
    GetDefaultSize(screenWidth, screenHeight);

    // シーンへ渡す共通リソースを設定する
    m_commonResources->Initialize(
        &m_timer,
        m_deviceResources.get(),
        m_commonStates.get(),
        m_debugString.get(),
        m_inputManager.get()
    );

    // シーンマネージャを初期化する
    m_sceneManager = std::make_unique<SceneManager>();
    m_sceneManager->Initialize(m_commonResources.get());
}

#pragma region Frame Update
// 基本的なゲームループを実行する
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// 世界を更新する
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // 入力マネージャを更新する
    m_inputManager->Update();

    // キーボードステートを取得する
    const auto& keyboardState = m_inputManager->GetKeyboardState();

    // 「ECS」キーで終了する
    if (keyboardState.Escape)
    {

        ExitGame();
    }

    // シーンマネージャを更新する
    m_sceneManager->Update(elapsedTime);
}
#pragma endregion

#pragma region Frame Render
// シーンを描く
void Game::Render()
{
    // 最初のUpdateの前に何かをレンダリングしようとしてはいけない。
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: レンダリングコードをここに追加する
    UNREFERENCED_PARAMETER(context);

    // デバッグ文字列を作成する：FPS
    //m_debugString->AddString("fps : %d", m_timer.GetFramesPerSecond());

    // シーンマネージャを描画する
    m_sceneManager->Render();

    // デバッグ文字列を描画する
    m_debugString->Render(m_commonStates.get());

    m_deviceResources->PIXEndEvent();

    // 新しいフレームを表示する.
    m_deviceResources->Present();
}

// バックバッファをクリアするヘルパーメソッド
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // 視界をクリアにする
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // ビューポートの設定
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// メッセージ・ハンドラ
void Game::OnActivated()
{
    // TODO: ゲームがアクティブウィンドウになる
}

void Game::OnDeactivated()
{
    // TODO: ゲームがバックグラウンド・ウィンドウになる
}

void Game::OnSuspending()
{
    // TODO: ゲームがパワーサスペンド（または最小化）されている。
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: 試合がパワーリスタートしている（またはミニマムから復帰している）
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
    // フルスクリーンか調べる
    BOOL fullscreen = FALSE;
    m_deviceResources->GetSwapChain()->GetFullscreenState(&fullscreen, nullptr);
    // フルスクリーンが解除されてしまった時の処理
    if (m_fullscreen != fullscreen)
    {
        m_fullscreen = fullscreen;
        // ResizeBuffers関数を呼び出す
        m_deviceResources->CreateWindowSizeDependentResources();
    }
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: ゲームウィンドウのサイズが変更されました。
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: 希望するデフォルトのウィンドウサイズに変更する（最小サイズは320x200であることに注意）.
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// これらはデバイスに依存するリソースである。
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: ここでデバイス依存オブジェクトを初期化する（ウィンドウサイズに依存しない）
    device;

    // 未使用の警告が出るのを防ぐ
    UNREFERENCED_PARAMETER(device);

}

// ウィンドウの SizeChanged イベントで変更されるすべてのメモリ リソースを割り当てます
void Game::CreateWindowSizeDependentResources()
{
    // TODO: ウィンドウサイズに依存するオブジェクトはここで初期化する
}

void Game::OnDeviceLost()
{
    // TODO: ここにDirect3Dリソースのクリーンアップを追加する
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}


#pragma endregion
