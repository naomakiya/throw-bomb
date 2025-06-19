//--------------------------------------------------------------------------------------
// File: SmorkUtil.cpp
//
// �p�[�e�B�N�����[�e�B���e�B�N���X
// position�⑬�x�Ȃǃp�[�e�B�N���P�ɕK�v�ȗv�f���i�[
// �X�V���ɐ������Ԃ���W�A�傫���Ȃǂ��Čv�Z���A
// �������s������false��Ԃ��č폜�𑣂�
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "SmorkUtility.h"

#include "Framework/StepTimer.h"
#include "Framework/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

using namespace DirectX;

const static float ENDLESS = -100.0f;

/// <summary>
/// �R���X�g���N�^
/// �����͏����ݒ�p�̐��l
/// </summary>
/// <param name="life">      ��������</param>
/// <param name="pos">       ���W</param>
/// <param name="velocity">  ���x</param>
/// <param name="accele">    �����x</param>
/// <param name="startScale">�����T�C�Y</param>
/// <param name="endScale">  �ŏI�T�C�Y</param>
/// <param name="startColor">�����F</param>
/// <param name="endColor">  �ŏI�F</param>
SmorkUtility::SmorkUtility(
	float life,
	DirectX::SimpleMath::Vector3 pos,
	DirectX::SimpleMath::Vector3 centerpos,
	DirectX::SimpleMath::Vector3 velocity,
	DirectX::SimpleMath::Vector3 accele,
	DirectX::SimpleMath::Vector3 startScale, DirectX::SimpleMath::Vector3 endScale,
	DirectX::SimpleMath::Color startColor, DirectX::SimpleMath::Color endColor)
{
	m_rotationAngle = 0;
	//	�^����ꂽ�������ԁis�j���擾
	m_startLife =
		m_life = life;

	//���݂̊p�x���擾
	m_initialAngle = 0.0f;
	//	�����ʒu���擾
	m_position = pos;
	m_centerPosition = centerpos;
	//	�ړ��x�N�g�����擾
	m_velocity = velocity;
	//	�����x�x�N�g�����擾
	m_accele = accele;

	//	���߂̃T�C�Y���擾
	m_startScale = startScale;
	m_nowScale = startScale;
	//	�I���̃T�C�Y���擾
	m_endScale = endScale;

	//	���߂̐F���擾
	m_startColor = startColor;
	m_nowColor = startColor;
	//	�I���̐F���擾
	m_endColor = endColor;
}
/// <summary>
/// �f�X�g���N�^
/// </summary>
SmorkUtility::~SmorkUtility()
{
}
/// <summary>
/// �X�V�֐�
/// </summary>
/// <param name="timer">Game������StepTimer���󂯎��</param>
/// <returns>��������(life)������Ԃ�true</returns>
bool SmorkUtility::Update(float elapsedTime, float radius)
{
	// �p�x�����Ԍo�߂ɉ����đ���������
	float rotationSpeed = 0.25f;  // ��]���x�i���W�A��/�b�j

	m_rotationAngle += rotationSpeed * elapsedTime;  // �p�x�X�V

	//	�X�P�[���B���݂̐������Ԃ���A�傫����Lerp�ŎZ�o����
	m_nowScale = SimpleMath::Vector3::Lerp(m_startScale, m_endScale, 1.0f - m_life / m_startLife);
	//	�F�B���݂̐������Ԃ���A�F��Lerp�ŎZ�o����
	m_nowColor = SimpleMath::Color::Lerp(m_startColor, m_endColor, 1.0f - m_life / m_startLife);

	float angle = m_initialAngle + m_rotationAngle;

	m_position.x = m_centerPosition.x + radius * cos(angle);
	m_position.z = m_centerPosition.z + radius * sin(angle);
	
	//	���C�t�����炵�Ă���
	m_life -= elapsedTime;

	//	���C�t��0�����Ȃ玩�g�������Ă��炤
	if (m_life < 0.0f)
	{
		return false;
	}

	return true;
}
