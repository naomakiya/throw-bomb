//--------------------------------------------------------------------------------------
// File: Star.cpp
//
// ���̃p�[�e�B�N���N���X
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "Star.h"
#include "Libraries/MyLib/BinaryFile.h"
#include "Framework/DeviceResources.h"
#include "StarUtility.h"
#include "Game/UI/UI.h"
#include "Libraries/MyLib/DebugString.h"
#include "Framework/CommonResources.h"

using namespace DirectX;

/// <summary>
/// �R���X�g���N�^
/// </summary>
Star::Star()
	:m_pDR(nullptr)
	,m_timer(0.0f)
	,m_textureIndex(0),
	m_position(SimpleMath::Vector3::Zero),
    m_Billboard{ DirectX::SimpleMath::Matrix::Identity }
	, m_on{false}
{
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
Star::~Star()
{
}

/// <summary>
/// �e�N�X�`�����\�[�X�ǂݍ��݊֐�
/// </summary>
/// <param name="path">���΃p�X(Resources/Textures/�E�E�E.png�Ȃǁj</param>
void Star::LoadTexture(const wchar_t* path)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	DirectX::CreateWICTextureFromFile(m_pDR->GetD3DDevice(), path, nullptr, texture.ReleaseAndGetAddressOf());
	
	m_texture.push_back(texture);
}

/// <summary>
/// �����֐�
/// </summary>
/// <param name="pDR">���[�U�[���\�[�X�����玝���Ă���</param>
void Star::Create(DX::DeviceResources* pDR)
{	
	m_pDR = pDR;
	ID3D11Device1* device = pDR->GetD3DDevice();

	//	�V�F�[�_�[�̍쐬
	CreateShader();

	//	�摜�̓ǂݍ���
	LoadTexture(L"Resources/Textures/Star.png");

	// �v���~�e�B�u�o�b�`�̍쐬
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColorTexture>>(pDR->GetD3DDeviceContext());

	m_states = std::make_unique<CommonStates>(device);

}
/// <summary>
/// �X�V�֐�
/// </summary>
/// <param name="timer">Game������StepTimer���󂯎��</param>
void Star::Update(float elapsedTime, DirectX::SimpleMath::Vector3 pos)
{
	m_timer += elapsedTime;

	if (m_on)
	{
		CreateCircularParticles(4, 0.5f, 1.0f, 0.1f, 2.0f, pos);
		m_on = false;
	}
	for (auto it = m_starUtility.begin(); it != m_starUtility.end();)
	{
		if (!it->Update(elapsedTime))
		{
			it = m_starUtility.erase(it);
		}
		else
		{
			++it;
		}
	}
	
}

/// <summary>
/// Shader�쐬�����������������֐�
/// </summary>
void Star::CreateShader()
{
	ID3D11Device1* device = m_pDR->GetD3DDevice();

	//	�R���p�C�����ꂽ�V�F�[�_�t�@�C����ǂݍ���
	BinaryFile VSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleVS.cso");
	BinaryFile GSData = BinaryFile::LoadFile(L"Resources/Shaders/ParticleStarGS.cso");
	BinaryFile PSData = BinaryFile::LoadFile(L"Resources/Shaders/StarPS.cso");
	
	//	�C���v�b�g���C�A�E�g�̍쐬
	device->CreateInputLayout(&UI::INPUT_LAYOUT[0],
		static_cast<UINT>(UI::INPUT_LAYOUT.size()),
		VSData.GetData(), VSData.GetSize(),
		m_inputLayout.GetAddressOf());

	//	���_�V�F�[�_�쐬
	if (FAILED(device->CreateVertexShader(VSData.GetData(), VSData.GetSize(), NULL, m_vertexShader.ReleaseAndGetAddressOf())))
	{// �G���[
		MessageBox(0, L"CreateVertexShader Failed.", NULL, MB_OK);
		return;
	}

	//	�W�I���g���V�F�[�_�쐬
	if (FAILED(device->CreateGeometryShader(GSData.GetData(), GSData.GetSize(), NULL, m_geometryShader.ReleaseAndGetAddressOf())))
	{// �G���[
		MessageBox(0, L"CreateGeometryShader Failed.", NULL, MB_OK);
		return;
	}
	//	�s�N�Z���V�F�[�_�쐬
	if (FAILED(device->CreatePixelShader(PSData.GetData(), PSData.GetSize(), NULL, m_pixelShader.ReleaseAndGetAddressOf())))
	{// �G���[
		MessageBox(0, L"CreatePixelShader Failed.", NULL, MB_OK);
		return;
	}

	//	�V�F�[�_�[�Ƀf�[�^��n�����߂̃R���X�^���g�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	device->CreateBuffer(&bd, nullptr, &m_CBuffer);
}

/// <summary>
/// �`��֐�
/// </summary>
/// <param name="view">�r���[�s��</param>
/// <param name="proj">�ˉe�s��</param>
void Star::Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj)
{
	ID3D11DeviceContext1* context = m_pDR->GetD3DDeviceContext();

	DirectX::SimpleMath::Vector3 cameraDir = m_cameraTarget - m_cameraPosition;
	cameraDir.Normalize();

	DirectX::SimpleMath::Vector3 containedCameraDir = cameraDir;
	containedCameraDir.y = 0.0f;
	containedCameraDir.Normalize();

	SimpleMath::Vector3 startDir = SimpleMath::Vector3::Forward;
	float dot = containedCameraDir.Dot(startDir);
	float cross = startDir.Cross(containedCameraDir).y;


	// �r���{�[�h�s����쐬
	XMVECTOR camPos = XMLoadFloat3(&m_cameraPosition);
	XMVECTOR camTarget = XMLoadFloat3(&m_cameraTarget);
	XMVECTOR look = XMVector3Normalize(camTarget - camPos);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, look));
	up = XMVector3Cross(look, right);

	XMMATRIX billboard = XMMATRIX(
		right,
		up,
		look,
		XMVectorSet(0, 0, 0, 1)
	);

	// �� ���̍s�� m_Billboard �ɃZ�b�g
	m_Billboard = billboard;

	//	�V�F�[�_�[�ɓn���ǉ��̃o�b�t�@���쐬����B(ConstBuffer�j
	ConstBuffer cbuff;
	cbuff.matView = view.Transpose();
	cbuff.matProj = proj.Transpose();
	cbuff.matWorld = m_world.Transpose();
	cbuff.Diffuse = SimpleMath::Vector4(dot, cross, 1, static_cast<float>(m_stepTimer.GetTotalSeconds()));

	//	�󂯓n���p�o�b�t�@�̓��e�X�V(ConstBuffer����ID3D11Buffer�ւ̕ϊ��j
	context->UpdateSubresource(m_CBuffer.Get(), 0, NULL, &cbuff, 0, 0);

	//	�V�F�[�_�[�Ƀo�b�t�@��n��
	ID3D11Buffer* cb[1] = { m_CBuffer.Get() };
	context->VSSetConstantBuffers(0, 1, cb);
	context->GSSetConstantBuffers(0, 1, cb);
	context->PSSetConstantBuffers(0, 1, cb);

	//	�摜�p�T���v���[�̓o�^
	ID3D11SamplerState* sampler[1] = { m_states->PointWrap() };
	context->PSSetSamplers(0, 1, sampler);

	//	�������`��w��
	ID3D11BlendState* blendstate = m_states->NonPremultiplied();

	//	�������菈��
	context->OMSetBlendState(blendstate, nullptr, 0xFFFFFFFF);

	//	�[�x�o�b�t�@�ɏ������ݎQ�Ƃ���
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

	//	�J�����O�͍�����
	context->RSSetState(m_states->CullNone());

	//	�V�F�[�_���Z�b�g����
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	for (int i = 0; i < m_texture.size(); i++)
	{
		context->PSSetShaderResources(i, 1, m_texture[i].GetAddressOf());
	}
	context->PSSetShaderResources(0, 1, m_texture[m_textureIndex].GetAddressOf());

	//	�C���v�b�g���C�A�E�g�̓o�^
	context->IASetInputLayout(m_inputLayout.Get());

	//	�|���S����`��
	m_batch->Begin();

	for (StarUtility& li : m_starUtility)
	{
		DirectX::SimpleMath::Vector3 pos = li.GetPosition();

		VertexPositionColorTexture vPCT{};
		vPCT.position = XMFLOAT3(pos);
		vPCT.color = XMFLOAT4(li.GetNowColor());
		vPCT.textureCoordinate = XMFLOAT2(li.GetNowScale().x, 0.0f);

		std::vector<VertexPositionColorTexture> oneVertex = { vPCT };
		m_batch->Draw(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, oneVertex.data(), 1);
	}
	
	m_batch->End();

	//	�V�F�[�_�̓o�^���������Ă���
	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);

}

/// <summary>
/// �p�[�e�B�N�����~�`�ɐ������A������ʒu�𒆐S�ɉ~�^��������
/// </summary>
/// <param name="numParticles">��������p�[�e�B�N���̐�</param>
/// <param name="radius">�~�̔��a</param>
/// <param name="speed">�p�[�e�B�N���̏����x</param>
void Star::CreateCircularParticles(int numParticles, float maxSpeed, float initialScale, float finalScale, float lifetime,DirectX::SimpleMath::Vector3 pos)
{
	float range = 10.0f;

	std::vector<DirectX::SimpleMath::Vector3> cornerPositions = {
	   {  range, 1.0f, 0.0f },   // �E��
	   {  0.0f,  1.0f, range },  // ����
	   { -range, 1.0f, 0.0f },   // �E��
	   { 0.0f,   1.0f, -range }  // ����
	};
	const float TWO_PI = DirectX::XM_2PI;   
	for (int i = 0; i < numParticles; ++i)
	{
		DirectX::SimpleMath::Vector3 initialPosition = pos + cornerPositions[i];

		// �ʑ��I�t�Z�b�g���v�Z
		float initialAngle = (TWO_PI / numParticles) * i;

		// ���x�𒆐S�����Ɍ�����i�O������Ɍ����������j
		DirectX::SimpleMath::Vector3 velocity = pos - initialPosition;
		//velocity.Normalize();
		velocity *= maxSpeed;

		// �p�[�e�B�N���𐶐�
		StarUtility pU(
			lifetime,  // lifetime
			initialPosition,
			pos,
			velocity,
			DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),  // �����x�Ȃ�
			DirectX::SimpleMath::Vector3(initialScale, initialScale, initialScale),
			DirectX::SimpleMath::Vector3(finalScale, finalScale, finalScale),
			DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f),  // ��
			DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 0.0f)   
		);
		// �� �p�x���d����
		pU.SetInitialAngle(initialAngle);

		m_starUtility.push_back(pU);
	}
}