/*
    @file	Camera.cpp
    @brief	�J�����N���X
*/
#pragma once
#ifndef CAMERA_DEFINED
#define CAMERA_DEFINED

class Camera {
public:
    //�J�����̉�]�p�x�̎擾
    DirectX::SimpleMath::Quaternion GetRotation() const { return m_rotationQuaternion; }
    // �r���[�}�g���b�N�X���擾
    DirectX::XMMATRIX GetViewMatrix() const { return m_view; }
public:
    //�R���X�g���N�g
    Camera();
    // �J�����̍X�V
    void Update(const DirectX::SimpleMath::Vector3& targetPosition);
    // �J�����̉�]�̋���
    void RotateCameraAroundPlayer(float angle);
    //�J�����̉�]
    void UpdateCameraRotation(float elapsedTime);
private:
    // �J�����̈ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �J�����̒����_
    DirectX::SimpleMath::Vector3 m_target;
    // �J�����̏�����x�N�g��
    DirectX::SimpleMath::Vector3 m_up;
    // �J�����̉E�����x�N�g��
    DirectX::SimpleMath::Vector3 m_right;
    // �J�����̃r���[�}�g���b�N�X
    DirectX::SimpleMath::Matrix m_view;
    //�N�D�[�^�j�I��
    DirectX::SimpleMath::Quaternion m_rotationQuaternion;
    // ��]���x
    float m_rotationSpeed;
    //ya����]
    float m_cameraYaw;
    //��]�p�x
    float  m_targetAngle;
    //���݂̊p�x
    float m_currentAngle;
    //�J������]�̋���
    bool m_isRotating;
private:
    // �^�[�Q�b�g����̃f�t�H���g����
    static constexpr float CAMERA_DISTANCE = 15.0f;
    //�J�����̍���
    static constexpr float CAMERA_HIGHT = 10.0f;
    // ��]���x
    static constexpr float CAMERA_ROTATIONSPEED = 1.5f; 
};
#endif  //CAMERA_DEFINED
