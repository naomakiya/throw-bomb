/*
    @file	Camera.cpp
    @brief	カメラクラス
*/
#include"pch.h"
#include "Camera.h"

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
Camera::Camera() :
    m_position(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
    m_target(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
    m_up(DirectX::SimpleMath::Vector3::UnitY),
    m_right(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f)),
    m_rotationQuaternion{},
    m_rotationSpeed(0.1f), // 回転速度を調整可能
    m_view{},
    m_cameraYaw{},
    m_targetAngle(DirectX::XM_PI),
    m_currentAngle(DirectX::XM_PI),
    m_isRotating(false)
{
    // Y軸回りに180度回転するクォータニオンを生成して適用
    m_rotationQuaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, DirectX::XM_PI);
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void Camera::Update(const DirectX::SimpleMath::Vector3& targetPosition)
{
      // ターゲット位置（プレイヤーの位置）を更新
    m_target = DirectX::SimpleMath::Vector3::Lerp(m_target, targetPosition, 0.1f);

    // 相対位置（カメラとプレイヤーの距離や高さを指定）
    DirectX::SimpleMath::Vector3 relativePosition = DirectX::SimpleMath::Vector3(0.0f, CAMERA_HIGHT, -CAMERA_DISTANCE);

    // クォータニオンから回転行列を作成し、相対位置に適用
    DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotationQuaternion);
    relativePosition = DirectX::SimpleMath::Vector3::Transform(relativePosition, rotationMatrix);

    // 新しいカメラ位置を計算（プレイヤーの位置 + 相対位置）を補間でスムーズ化
    DirectX::SimpleMath::Vector3 desiredPosition = targetPosition + relativePosition;
    m_position = DirectX::SimpleMath::Vector3::Lerp(m_position, desiredPosition, 0.1f); // 0.1fで緩やかに補間

    // カメラのビュー行列を更新（カメラが常にプレイヤーを向くように）
    m_view = DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_target, m_up);

}

//---------------------------------------------------------
// カメラの回転の許可
//---------------------------------------------------------
void Camera::RotateCameraAroundPlayer(float angle)
{
    // 回転中であれば新しい回転を無視
    if (m_isRotating) return;
    
    // ボタンが押されたときに目標角度をセット
    m_targetAngle += angle; // 現在の目標に加算する形で設定

    // 回転を開始するフラグをオンにする
    m_isRotating = true;

}

//---------------------------------------------------------
// カメラの回転
//---------------------------------------------------------
void Camera::UpdateCameraRotation(float elapsedTime)
{
    // 回転中でない場合は何もしない
    if (!m_isRotating) return;

    // 現在の角度と目標角度の差分を計算
    float angleDifference = m_targetAngle - m_currentAngle;

    // 目標角度に少しずつ近づける
    float deltaAngle = CAMERA_ROTATIONSPEED * elapsedTime;

    // 角度差がまだ大きい場合、少しずつ回転を進める
    if (fabs(angleDifference) > deltaAngle)
    {
        // 回転角度を進める
        m_currentAngle += (angleDifference > 0) ? deltaAngle : -deltaAngle;
    }
    else
    {
        // 目標角度に到達したら回転を終了
        m_currentAngle = m_targetAngle;
        m_isRotating = false;
    }
    // 回転を反映
    DirectX::SimpleMath::Quaternion rotationQuat = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::UnitY, m_currentAngle);
    m_rotationQuaternion = rotationQuat;
}