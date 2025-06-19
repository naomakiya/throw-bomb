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

// ��{�I�ȃQ�[�������ŁAD3D11�f�o�C�X��
// �Q�[�����[�v��񋟂���
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // �������ƊǗ�
    void Initialize(HWND window, int width, int height);

    // ��{�I�Ȏ����̃��[�v
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // ���b�Z�[�W
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // �v���p�e�B
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

    // �f�o�C�X�̃��\�[�X
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // �E�B���h�E�n���h��
    HWND                                    m_hWnd;

    //�����_�����O���[�v�^�C�}�[
    DX::StepTimer                           m_timer;

    // �R�����X�e�[�g
    std::unique_ptr<DirectX::CommonStates>  m_commonStates;

    // �R�������\�[�X
    std::unique_ptr<CommonResources>        m_commonResources;

    // �f�o�b�O�X�g�����O
    std::unique_ptr<mylib::DebugString>     m_debugString;

    // ���̓}�l�[�W��
    std::unique_ptr<mylib::InputManager>    m_inputManager;

    // �V�[���}�l�[�W��
    std::unique_ptr<SceneManager>           m_sceneManager;

    //�t���X�N���[���ɂ��邩���Ȃ���
    BOOL                                    m_fullscreen;

};
