/*
	@file	HollowHartUI.cpp
	@brief	��̃n�[�g�N���X
*/
#include "pch.h"
#include "Game/UI/Hart/HollowHartUI.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include <Game/Screen.h>


//---------------------------------------------------------
// �R���X�g���N�^
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
// �f�X�g���N�^
//---------------------------------------------------------
HollowHartUI::~HollowHartUI()
{
	
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void HollowHartUI::Initialize(CommonResources* resources, DirectX::DX11::SpriteBatch* spriteBatch)
{
    using namespace DirectX;

	assert(resources);
    assert(spriteBatch);
	m_commonResources = resources;
	m_spriteBatch = spriteBatch;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    // �摜�����[�h����
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(
            device,
            L"Resources/Textures/hartEmpty.png",
            nullptr,
            m_texture.ReleaseAndGetAddressOf()
        )
    );

    // �ꎞ�I�ȕϐ��̐錾
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
    D3D11_TEXTURE2D_DESC desc;

    // �e�N�X�`���̏����擾����
    m_texture->GetResource(resource.GetAddressOf());
    resource.As(&tex2D);
    tex2D->GetDesc(&desc);

    // �e�N�X�`���T�C�Y���擾���Afloat�^�ɕϊ�����
    m_texSize.x = static_cast<float>(desc.Width);
    m_texSize.y = static_cast<float>(desc.Height);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void HollowHartUI::Render(const int index)
{
    using namespace DirectX;

    // ��n�[�g�̏����ʒu�̐ݒ�
    float startPos = Screen::CENTER_X - m_texSize.x;

    // ��n�[�g�`��ʒu�����߂�
    SimpleMath::Vector2 pos(startPos + m_texSize.x *(2 - index), Screen::TOP );

    // �n�[�g��`�悷��
    m_spriteBatch->Draw(m_texture.Get(), pos);
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void HollowHartUI::Finalize()
{
	// do nothing.
}
