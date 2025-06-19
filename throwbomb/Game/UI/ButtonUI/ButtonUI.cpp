/*
	@file	ButtonUI.cpp
	@brief	�{�^���Ɋւ��郆�[�U�[�C���^�[�t�F�C�X�N���X
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
// �R���X�g���N�^
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
// �f�X�g���N�^
//---------------------------------------------------------
ButtonUI::~ButtonUI()
{
	
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void ButtonUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
{
    assert(resources);
    m_commonResources = resources;
    m_spriteBatch = spriteBatch;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    // �摜�����[�h���� (���)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/arrow.png",
            nullptr,
            m_arrowTexture.ReleaseAndGetAddressOf()
        )
    );

    // �摜�����[�h���� (�{�^��A)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/BttunA.png",
            nullptr,
            m_ButtonUITextureA.ReleaseAndGetAddressOf()
        )
    );
    // �摜�����[�h���� (�{�^��D)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/BttunD.jpg",
            nullptr,
            m_ButtonUITextureD.ReleaseAndGetAddressOf()
        )
    );
    // �摜�����[�h���� (�J����)
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/Camera.png",
            nullptr,
            m_cameraUITexture.ReleaseAndGetAddressOf()
        )
    );

    // �e�N�X�`���̒��S�ʒu��ݒ�
    m_texCenter = Vector2(128.0f, 128.0f); // �e�N�X�`���̒��S (256x256 �̃e�N�X�`����z��)

}



//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void ButtonUI::Render()
{
    // �Q�[�W�̕�����`��
    m_spriteBatch->Draw(
        m_ButtonUITextureA.Get(),            // �e�N�X�`��
        Vector2(25, 650),             // �ʒu
        nullptr,                        // �\�[�X��`
        Colors::White,                  // �F
        0,                      // ��]�p�x
        Vector2(0, 0),              // ��]�̒��S
        0.25f,                           // �X�P�[��
        SpriteEffects_None,             // �X�v���C�g�G�t�F�N�g
        0.0f                            // ���C���[�[�x
    );
    //���e�N�X�`���[
    m_spriteBatch->Draw(
        m_arrowTexture.Get(),            // �e�N�X�`��
        Vector2(125, 675),             // �ʒu
        nullptr,                        // �\�[�X��`
        Colors::White,                  // �F
        0,                         // ��]�p�x
        Vector2(0, 0),              // ��]�̒��S
        0.05f,                           // �X�P�[��
        SpriteEffects_None,             // �X�v���C�g�G�t�F�N�g
        0.0f                            // ���C���[�[�x
    );
    //�J�����e�N�X�`���[
    m_spriteBatch->Draw(
        m_cameraUITexture.Get(),            // �e�N�X�`��
        Vector2(100, 550),             // �ʒu
        nullptr,                        // �\�[�X��`
        Colors::White,                  // �F
        0,                         // ��]�p�x
        Vector2(0, 0),              // ��]�̒��S
        0.15f,                           // �X�P�[��
        SpriteEffects_None,             // �X�v���C�g�G�t�F�N�g
        0.0f                            // ���C���[�[�x
    );
    //D�{�^���e�N�X�`���[
    m_spriteBatch->Draw(
        m_ButtonUITextureD.Get(),            // �e�N�X�`��
        Vector2(275, 650),             // �ʒu
        nullptr,                        // �\�[�X��`
        Colors::White,                  // �F
        0,                      // ��]�p�x
        Vector2(0, 0),              // ��]�̒��S
        0.25f,                           // �X�P�[��
        SpriteEffects_None,             // �X�v���C�g�G�t�F�N�g
        0.0f                            // ���C���[�[�x
    );
    
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void ButtonUI::Finalize()
{
	// do nothing.
}

