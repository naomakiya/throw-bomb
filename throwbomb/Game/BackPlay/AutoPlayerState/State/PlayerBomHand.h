/*
  @file 　PlayerBomHand.h
  @brief プレイヤボム持ち状態クラス
*/
#pragma once
#ifndef PLAYERBOMHAND_DEFINED
#define PLAYERBOMHAND_DEFINED
#include"Interface/IPlayerState.h"

class CommonResources;
class PlayerState;
class Wall;
class Camera;

class PlayerBomHand :public IPlayerState
{
public:
	// コンストラクタ
	PlayerBomHand(Camera* camera,PlayerState* playerState, const std::vector<std::unique_ptr<Wall>>& wall);
	//デストラクタ
	~PlayerBomHand();
	//初期化
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
	//移動
	void Movement(const float& elapsedTime);
private:
	// 共通リソース
	CommonResources* m_commonResources;
	//カメラ
	Camera* m_camera;
	//壁
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	//プレイシーン
	PlayerState* m_player;
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
	// クォータニオン：モデルの回転を制御する
	DirectX::SimpleMath::Quaternion m_rotate;
	// バウンディングスフィア
	DirectX::BoundingSphere m_boundingSphere;
	//手を動かす
	float m_hand;
	//時間
	float m_time;
};
#endif		// PLAYER_DEFINED