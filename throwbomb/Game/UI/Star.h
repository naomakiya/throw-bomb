//--------------------------------------------------------------------------------------
// File: Particle.h
//
// �p�[�e�B�N���N���X
// ���g���̏ꍇ��Position���̂�
//
//-------------------------------------------------------------------------------------

#pragma once
#include "Framework/DeviceResources.h"
#include <list>
#include"StarUtility.h"

class CommonResources;

class Star
{
public:
		//	�f�[�^�󂯓n���p�R���X�^���g�o�b�t�@(���M��)
		struct ConstBuffer
		{
			DirectX::SimpleMath::Matrix		matWorld;
			DirectX::SimpleMath::Matrix		matView;
			DirectX::SimpleMath::Matrix		matProj;
			DirectX::SimpleMath::Vector4	Diffuse;
		};
		void SetOn(bool on) { m_on = on; }
public:
	Star();
	~Star();

	void LoadTexture(const wchar_t* path);

	void Create(DX::DeviceResources* pDR);

	void Update(float elapsedTime,DirectX::SimpleMath::Vector3 pos);

	void Render(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

	void CreateShader();

	void CreateCircularParticles(int numParticles, float maxSpeed, float initialScale, float finalScale, float lifetime, DirectX::SimpleMath::Vector3 pos);
private:
		//	�ϐ�
		DX::DeviceResources* m_pDR;
		//�ʒu
		DirectX::SimpleMath::Vector3 m_position;
		float m_timer;
		//�r���{�[�h�쐬�p�ϐ�
		DirectX::SimpleMath::Matrix m_Billboard;

		Microsoft::WRL::ComPtr<ID3D11Buffer>	m_CBuffer;

		//	���̓��C�A�E�g
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		//	�v���~�e�B�u�o�b�`
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColorTexture>> m_batch;
		//	�R�����X�e�[�g
		std::unique_ptr<DirectX::CommonStates> m_states;
		//	�e�N�X�`���n���h��
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_texture;
		//	�e�N�X�`���n���h��
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;
		//	���_�V�F�[�_
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
		//	�s�N�Z���V�F�[�_
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
		//	�W�I���g���V�F�[�_
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;

		DirectX::SimpleMath::Matrix m_world;
		DirectX::SimpleMath::Matrix m_view;
		DirectX::SimpleMath::Matrix m_proj;

		DirectX::SimpleMath::Matrix m_billboard;

		std::vector<DirectX::VertexPositionColorTexture> m_vertices;
		std::list<StarUtility> m_starUtility;
		
		DirectX::SimpleMath::Vector3 m_cameraPosition;
		DirectX::SimpleMath::Vector3 m_cameraTarget;

		DX::StepTimer m_stepTimer;
		int m_textureIndex;

		bool m_on;


};