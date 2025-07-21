/*
	@file	Floor.cpp
	@brief	���N���X
*/
#include "pch.h"
#include "Floor.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"

//---------------------------------------------------------
// �R���X�g���N�g
//---------------------------------------------------------
Floor::Floor()
	:
	m_commonResources{},
	m_position{DirectX::SimpleMath::Vector3::Zero}
{
}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
Floor::~Floor()
{
}

//---------------------------------------------------------
// ������
//---------------------------------------------------------
void Floor::Initialize(CommonResources* resources)
{
	assert(resources);

	// ���ʃ��\�[�X�̐ݒ�
	m_commonResources = resources;
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	// �e�N�X�`���̃��[�h
	DirectX::CreateWICTextureFromFile(
		device,
		ResourceManager::GetTexturePath("Floor").c_str(), 
		nullptr,
		m_Texture.GetAddressOf()
	);
	// �{�b�N�X���쐬�i�T�C�Y�F���A�[���A���ɔ��������j
	DirectX::XMFLOAT3 size(FLOORWIDTH, 1.0f,FLOORDEPT); 
	// �����̂̍쐬
	m_floorPrimitive = DirectX::GeometricPrimitive::CreateBox(context, size);
	// �o�E���f�B���O�{�b�N�X�̍쐬
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = size;
}




//---------------------------------------------------------
// �`��
//---------------------------------------------------------
void Floor::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	using namespace DirectX;

	// ���[���h�s��i���̈ʒu�j
	SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::CreateTranslation(m_position);
	// �W�I���g���b�N�v���~�e�B�u�̕`��
	m_floorPrimitive->Draw(worldMatrix, view, proj, Colors::White, m_Texture.Get());
}


//---------------------------------------------------------
// ����
//---------------------------------------------------------
void Floor::Finalize()
{

}
