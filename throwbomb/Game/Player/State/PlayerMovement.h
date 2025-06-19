/*
  @file 　PlayerMovement.h
  @brief プレイヤ移動状態クラス
*/
#pragma once
#ifndef PLAYERMOVEMENT_DEFINED
#define PLAYERMOVEMENT_DEFINED
#include"Interface/IPlayerState.h"

class CommonResources;
class PlayerState;
class Camera;
class Wall;

class PlayerMovement :public IPlayerState
{
public:
	// コンストラクタ
	PlayerMovement(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
	//デストラクタ
	~PlayerMovement();
	//初期化する
	void Initialize(CommonResources* resources);
	// 事前更新する
	void PreUpdate();
	//更新する
	void Update(const float& elapsedTime);
	// 事後更新する
	void PostUpdate();
	//描画する
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) ;
	//後処理を行う
	void Finalize();
	//移動処理
	void Movement(const float& elapsedTime);
public:
	// 速さ
	static constexpr float STARTSPEED = 1.0f;
	//最大速度
	static constexpr float MAXSPEED = 5.0f;  
	// 加速度
	const float acceleration = 0.15f;  
private:
	//プレイヤー
	PlayerState* m_player;
	//壁
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	// 共通リソース
	CommonResources* m_commonResources;
	//現在の状態
	IPlayerState* m_currentState;
	//カメラ
	Camera* m_camera;
    // クォータニオン：モデルの回転を制御する
    DirectX::SimpleMath::Quaternion m_rotate;
    // 角度
    DirectX::SimpleMath::Vector3 m_angle;
    // スケール
    DirectX::SimpleMath::Vector3 m_scale;
	//位置
	DirectX::SimpleMath::Vector3 m_postion;
	// Playerの顔モデル
	std::unique_ptr<DirectX::Model> m_playerface;
	// Playerのモデル
	std::unique_ptr<DirectX::Model> m_playerfaceBody;
	// Playerの左手モデル
	std::unique_ptr<DirectX::Model> m_playerHandL;
	// Playerの右手モデル
	std::unique_ptr<DirectX::Model> m_playerHandR;
	//手を動かす
	float m_hand;
	//腕の振り子
	float m_handAngle;
	//速度
	float m_vel;
	//時間
	float m_time;

	
};
#endif		// PLAYER_DEFINED