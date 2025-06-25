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
    // �f�o�C�X���\�[�X���擾����
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // �f�o�C�X�̒ʒm���ݒ肷��
    //m_deviceResources->RegisterDeviceNotify(this);
}

// ���s�ɕK�v��Direct3D���\�[�X������������B
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

     //�f�o�C�X�ƃR���e�L�X�g���擾����
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // ���̓}�l�[�W�����쐬����
    m_inputManager = std::make_unique<mylib::InputManager>(window);

    // �R�����X�e�[�g���쐬����
    m_commonStates = std::make_unique<CommonStates>(device);

    // �f�o�b�O��������쐬����
    m_debugString = std::make_unique<mylib::DebugString>(
        device,
        context,
        L"Resources/Fonts/SegoeUI_18.spritefont"
    );

    // ���ʃ��\�[�X���쐬����
    m_commonResources = std::make_unique<CommonResources>();

    // ��ʃT�C�Y
    int screenWidth, screenHeight;

    // ��ʃT�C�Y���擾����
    GetDefaultSize(screenWidth, screenHeight);

    // �V�[���֓n�����ʃ��\�[�X��ݒ肷��
    m_commonResources->Initialize(
        &m_timer,
        m_deviceResources.get(),
        m_commonStates.get(),
        m_debugString.get(),
        m_inputManager.get()
    );

    // �V�[���}�l�[�W��������������
    m_sceneManager = std::make_unique<SceneManager>();
    m_sceneManager->Initialize(m_commonResources.get());
}

#pragma region Frame Update
// ��{�I�ȃQ�[�����[�v�����s����
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// ���E���X�V����
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // ���̓}�l�[�W�����X�V����
    m_inputManager->Update();

    // �L�[�{�[�h�X�e�[�g���擾����
    const auto& keyboardState = m_inputManager->GetKeyboardState();

    // �uECS�v�L�[�ŏI������
    if (keyboardState.Escape)
    {

        ExitGame();
    }

    // �V�[���}�l�[�W�����X�V����
    m_sceneManager->Update(elapsedTime);
}
#pragma endregion

#pragma region Frame Render
// �V�[����`��
void Game::Render()
{
    // �ŏ���Update�̑O�ɉ����������_�����O���悤�Ƃ��Ă͂����Ȃ��B
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: �����_�����O�R�[�h�������ɒǉ�����
    UNREFERENCED_PARAMETER(context);

    // �f�o�b�O��������쐬����FFPS
    //m_debugString->AddString("fps : %d", m_timer.GetFramesPerSecond());

    // �V�[���}�l�[�W����`�悷��
    m_sceneManager->Render();

    // �f�o�b�O�������`�悷��
    m_debugString->Render(m_commonStates.get());

    m_deviceResources->PIXEndEvent();

    // �V�����t���[����\������.
    m_deviceResources->Present();
}

// �o�b�N�o�b�t�@���N���A����w���p�[���\�b�h
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // ���E���N���A�ɂ���
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // �r���[�|�[�g�̐ݒ�
    auto const viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// ���b�Z�[�W�E�n���h��
void Game::OnActivated()
{
    // TODO: �Q�[�����A�N�e�B�u�E�B���h�E�ɂȂ�
}

void Game::OnDeactivated()
{
    // TODO: �Q�[�����o�b�N�O���E���h�E�E�B���h�E�ɂȂ�
}

void Game::OnSuspending()
{
    // TODO: �Q�[�����p���[�T�X�y���h�i�܂��͍ŏ����j����Ă���B
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: �������p���[���X�^�[�g���Ă���i�܂��̓~�j�}�����畜�A���Ă���j
}

void Game::OnWindowMoved()
{
    auto const r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
    // �t���X�N���[�������ׂ�
    BOOL fullscreen = FALSE;
    m_deviceResources->GetSwapChain()->GetFullscreenState(&fullscreen, nullptr);
    // �t���X�N���[������������Ă��܂������̏���
    if (m_fullscreen != fullscreen)
    {
        m_fullscreen = fullscreen;
        // ResizeBuffers�֐����Ăяo��
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

    // TODO: �Q�[���E�B���h�E�̃T�C�Y���ύX����܂����B
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: ��]����f�t�H���g�̃E�B���h�E�T�C�Y�ɕύX����i�ŏ��T�C�Y��320x200�ł��邱�Ƃɒ��Ӂj.
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// �����̓f�o�C�X�Ɉˑ����郊�\�[�X�ł���B
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: �����Ńf�o�C�X�ˑ��I�u�W�F�N�g������������i�E�B���h�E�T�C�Y�Ɉˑ����Ȃ��j
    device;

    // ���g�p�̌x�����o��̂�h��
    UNREFERENCED_PARAMETER(device);

}

// �E�B���h�E�� SizeChanged �C�x���g�ŕύX����邷�ׂẴ����� ���\�[�X�����蓖�Ă܂�
void Game::CreateWindowSizeDependentResources()
{
    // TODO: �E�B���h�E�T�C�Y�Ɉˑ�����I�u�W�F�N�g�͂����ŏ���������
}

void Game::OnDeviceLost()
{
    // TODO: ������Direct3D���\�[�X�̃N���[���A�b�v��ǉ�����
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}


#pragma endregion
