/*
    @file	Camera.cpp
    @brief	カメラクラス
*/
#pragma once
#ifndef CAMERA_DEFINED
#define CAMERA_DEFINED

class Camera {
public:
    //カメラの回転角度の取得
    DirectX::SimpleMath::Quaternion GetRotation() const { return m_rotationQuaternion; }
    // ビューマトリックスを取得
    DirectX::XMMATRIX GetViewMatrix() const { return m_view; }
public:
    //コンストラクト
    Camera();
    // カメラの更新
    void Update(const DirectX::SimpleMath::Vector3& targetPosition);
    // カメラの回転の許可
    void RotateCameraAroundPlayer(float angle);
    //カメラの回転
    void UpdateCameraRotation(float elapsedTime);
private:
    // カメラの位置
    DirectX::SimpleMath::Vector3 m_position;
    // カメラの注視点
    DirectX::SimpleMath::Vector3 m_target;
    // カメラの上方向ベクトル
    DirectX::SimpleMath::Vector3 m_up;
    // カメラの右方向ベクトル
    DirectX::SimpleMath::Vector3 m_right;
    // カメラのビューマトリックス
    DirectX::SimpleMath::Matrix m_view;
    //クゥータニオン
    DirectX::SimpleMath::Quaternion m_rotationQuaternion;
    // 回転速度
    float m_rotationSpeed;
    //yaを回転
    float m_cameraYaw;
    //回転角度
    float  m_targetAngle;
    //現在の角度
    float m_currentAngle;
    //カメラ回転の許可
    bool m_isRotating;
private:
    // ターゲットからのデフォルト距離
    static constexpr float CAMERA_DISTANCE = 15.0f;
    //カメラの高さ
    static constexpr float CAMERA_HIGHT = 10.0f;
    // 回転速度
    static constexpr float CAMERA_ROTATIONSPEED = 1.5f; 
};
#endif  //CAMERA_DEFINED
