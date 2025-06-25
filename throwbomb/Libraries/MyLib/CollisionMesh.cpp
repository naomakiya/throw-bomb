/*
	@file	CollisionMesh.cpp
	@brief	Obj�`���̃��b�V�����R���W�����ɂ���N���X
*/
#include "pch.h"
#include "CollisionMesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <Game/ResourceManager/ResourceManager.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

//-----------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------
mylib::CollisionMesh::CollisionMesh(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Quaternion rotation)
	:
	m_position{ pos },
	m_rotation{ rotation },
	m_vertices{},
	m_indices{},
	m_triangles{},
	m_inputLayout{},
	m_basicEffect{},
	m_primitiveBatch{},
	m_model{  }
{
}

//-----------------------------------------------------
// ����������
//-----------------------------------------------------
void mylib::CollisionMesh::Initialize(
	ID3D11Device* device,
	const std::wstring& objName,
	int typeValue
)
{
	assert(device);

	// Obj�t�@�C����ǂݍ���
	this->LoadObjFile(objName);

	// �|���S���z����쐬����
	for (size_t i = 0; i < m_indices.size() / 3; i++)
	{
		this->AddTriangle(
			m_vertices[m_indices[i * 3 + 0]].position,
			m_vertices[m_indices[i * 3 + 1]].position,
			m_vertices[m_indices[i * 3 + 2]].position
		);
	}
	//��ނɂ���ăI�u�W�F�N�g�̓ǂݍ��݂�ύX
	if (1 == typeValue)
	{
		//�ǂݍ��ݏ���
		std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
		fx->SetDirectory(L"Resources/Models");
		//�ǃ��f���̓ǂݎ��
		std::shared_ptr<DirectX::Model> CollisionMeshsModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("stairs1").c_str(), *fx);
		if (CollisionMeshsModel == nullptr)
		{
			assert(CollisionMeshsModel);
		}

		m_model = CollisionMeshsModel;
	}
	else if (2 == typeValue)
	{
		//�ǃ��f���̓ǂݎ��
		std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
		fx->SetDirectory(L"Resources/Models");
		std::shared_ptr<DirectX::Model> second = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("stairs2").c_str(), *fx);
	
		m_model = second;
	}
}

//-----------------------------------------------------
// �`�悷��
//-----------------------------------------------------
void mylib::CollisionMesh::Draw(
	ID3D11DeviceContext* context,
	DirectX::CommonStates* states,
	const DirectX::SimpleMath::Matrix& view,
	const DirectX::SimpleMath::Matrix& projection
)
{
	assert(context);
	assert(states);

	// ���[���h�s����v�Z����
	m_rotation.Normalize();

	// ��]�̕␳���s�� (�t��]�̏C��)
	Matrix rotation = Matrix::CreateFromQuaternion(m_rotation);
	Matrix inverseRotation = Matrix::CreateFromAxisAngle(Vector3::UnitY, XMConvertToRadians(180.0f)) * rotation;

	// �ʒu�ƃX�P�[����K�p
	Matrix translation = Matrix::CreateTranslation(m_position);
	Matrix world = Matrix::CreateScale(Vector3(0.05f, 0.1f, 0.10f)) * inverseRotation * translation;

	// ���f����\������
  	m_model->Draw(context, *states, world, view, projection);

	//���.obj�̕`�悷�邽�߂̏���
	//// �f�o�C�X�R���e�L�X�g�̃p�����[�^��ݒ肷��
	//context->OMSetBlendState(states->AlphaBlend(), nullptr, 0xFFFFFFFF);
	//context->OMSetDepthStencilState(states->DepthRead(), 0);	// �X�e���V���o�b�t�@�[�֏������܂Ȃ�
	//context->RSSetState(states->Wireframe());
	//context->IASetInputLayout(m_inputLayout.Get());
	 
	//// �x�[�V�b�N�G�t�F�N�g�̃p�����[�^��ݒ肷��
	//m_basicEffect->SetWorld(world);
	//m_basicEffect->SetView(view);
	//m_basicEffect->SetProjection(projection);
	//m_basicEffect->Apply(context);

	//// ���b�V����`�悷��
	//m_primitiveBatch->Begin();
	//m_primitiveBatch->DrawIndexed(
	//	D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	//	&m_indices[0], m_indices.size(),
	//	&m_vertices[0], m_vertices.size()
	//);
	//m_primitiveBatch->End();
}

bool mylib::CollisionMesh::IntersectRay(
	const DirectX::SimpleMath::Ray& ray,        // ���C
	DirectX::SimpleMath::Vector3* hitPosition,  // �Փˍ��W
	DirectX::SimpleMath::Vector3* normal,       // �Փ˂����|���S���̖@��
	int* polygonNo                              // �Փ˂����|���S���̔ԍ�
)
{
	assert(hitPosition);
	assert(normal);

	// �R���W�������b�V���̉�]����W��񂩂�Aworld�̋t�s������߂�
	Matrix rotation = Matrix::CreateFromQuaternion(m_rotation);
	Matrix transration = Matrix::CreateTranslation(m_position);
	Matrix world = rotation * transration;
	Matrix invertWorld = world.Invert();

	// ���C�����[�J�����W�n�ɕϊ�
	Ray unitRay{};
	unitRay.position = Vector3::Transform(ray.position, invertWorld);
	unitRay.direction = Vector3::TransformNormal(ray.direction, invertWorld);
	unitRay.direction.Normalize();

	float distance = 0.0f;
	bool hitDetected = false;
	float closestDistance = FLT_MAX;

	// 1. �O�p�`�|���S���Ƃ̏Փ˔���
	for (int i = 0; i < m_triangles.size(); i++)
	{
		Vector3 triangleNormal = m_triangles[i].normal;
		triangleNormal.Normalize();

		// ���C�ƃ|���S���̏Փ˔���
		if (unitRay.Intersects(
			m_triangles[i].triangle[0],
			m_triangles[i].triangle[1],
			m_triangles[i].triangle[2],
			distance))
		{
			if (distance < closestDistance) // �ł��߂��Փ˓_��D��
			{
				closestDistance = distance;
				*hitPosition = Vector3{ unitRay.position + unitRay.direction * distance };
				*hitPosition = Vector3::Transform(*hitPosition, world);

				// �@���̉�]
				*normal = Vector3::TransformNormal(triangleNormal, rotation); 
				*polygonNo = i;
				hitDetected = true;
			}
		}
	}

	
	return hitDetected;
}
//-----------------------------------------------------
// Obj�`���̃t�@�C�������[�h����
//-----------------------------------------------------
void mylib::CollisionMesh::LoadObjFile(const std::wstring& objName)
{
	// Obj�`���̃t�@�C�����J��
	std::wstring fileName = L"Resources/Models/" + objName + L".obj";
	std::ifstream ifs(fileName);

	assert(ifs && ".obj�t�@�C�����ǂݍ��߂܂���ł���");

	// 1�s���Ƃɏ�������
	std::string line;
	while (getline(ifs, line))
	{
		// �ǂݎ�����P�s���̃f�[�^���X�g�����O�X�g���[���ň���
		std::stringstream iss(line);
		std::string type;

		// �擪�v�f���󂯎��
		iss >> type;

		// ���_�f�[�^���쐬����
		// ��Obj�f�[�^��vt,vn�����鎞������̂Œ��ӂ���
		if (type.compare("v") == 0)
		{
			Vector3 vec;
			iss >> vec.x >> vec.y >> vec.z;
			m_vertices.emplace_back(vec);
		}
		// �C���f�b�N�X�f�[�^���쐬����
		else if (type.compare("f") == 0)
		{
			int a, b, c;
			iss >> a >> b >> c;

			// �O�p�`�̒��_�C���f�b�N�X�ԍ������������܂ޏꍇ�͖�������F�R�_���قȂ�_���w���Ă���
			if (a != b && a != c && b != c)
			{
				/*
					�EObj�f�[�^�̃|���S���̒��_�͔����v���ɋL�^����Ă���
					�E������W�n�ł͒��_�͎��v���Ɏw�肷��
					�EDirectXTK�ł�������g�p����ꍇ�͖@����񂪔��]����̂ŁA���ӂ��K�v
				*/
				/*m_indices.push_back(static_cast<uint16_t>(a - 1));
				m_indices.push_back(static_cast<uint16_t>(c - 1));
				m_indices.push_back(static_cast<uint16_t>(b - 1));*/

				/*
					�EObj�f�[�^�̃|���S���̒��_�͔����v���ɋL�^����Ă���
					�E�E����W�n�ł͂��̂ق����s�����悢�̂ŁA���̂܂ܗ��p����
					�E�@�������߂�ۂ����]���Ȃ��Ă悢
				*/
				m_indices.emplace_back(static_cast<uint16_t>(a - 1));
				m_indices.emplace_back(static_cast<uint16_t>(b - 1));
				m_indices.emplace_back(static_cast<uint16_t>(c - 1));
			}
		}
	}

	// �J�����t�@�C�������
	ifs.close();
}

//-----------------------------------------------------
// �O�p�`�|���S�������A�|���S���z��ɓo�^����
//-----------------------------------------------------
void mylib::CollisionMesh::AddTriangle(
	DirectX::SimpleMath::Vector3 a,
	DirectX::SimpleMath::Vector3 b,
	DirectX::SimpleMath::Vector3 c
)
{
	m_triangles.emplace_back(Triangle(a, b, c));
}
