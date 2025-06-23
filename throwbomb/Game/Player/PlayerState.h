/*
  @file　PlayerState.h
  @brief プレイヤ状態クラス
*/

#pragma once
#ifndef PLAYERSTATE_DEFINED
#define PLAYERSTATE_DEFINED
#include"Interface/IPlayerState.h"

// 前方宣言
class CommonResources;
class PlayerScene;
class PlayerIdling;
class PlayerBomHand;
class PlayerMovement;
class BomState;
class Wall;
class Camera;
class CollisionDebugRenderer;
class Shadow;
class Floor;

class PlayerState : public IPlayerState
{
public:
	// 角度を取得する
	DirectX::SimpleMath::Quaternion GetAngle() const { return m_rotate; }
	// 角度を設定する
	void SetAngle(const DirectX::SimpleMath::Quaternion& rotate) { m_rotate = rotate; }
	// バウンディングボックスを取得する
	DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
	// 位置を取得する
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 位置を設定する
	void SetPosition(const DirectX::SimpleMath::Vector3& position){
		m_position = position;	m_boundingBox.Center = m_position;
	}
	// ボムステートを取得する
	BomState* GetBomState(int index) { if (index >= 0 && index < m_bom.size()) { return m_bom[index].get(); } return nullptr;	}
	// 現在の向いている方向を取得する
	DirectX::SimpleMath::Vector3 GetForwardDirection() const {return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate);}
	// 敵との当たり判定の取得
	bool GetHitEnemy() const { return m_isEnemyHit; }
	// 敵との当たり判定の設定
	void SetHitEnemy(const bool& isHitEnemy) {
		m_isEnemyHit = isHitEnemy;
		m_hitTimer = 0.0f;
		m_flashTimer = 0.0f;
	}
	// 存在状態の取得
	bool GetExist() const { return m_exist; }
	// フラッシュ状態の取得
	bool GetFlashing() const { return m_isFlashing; }
	// プレイヤーのアイドリングを取得する
	PlayerIdling* GetPlayerIdling() const { return m_playerIdling.get(); }
	// プレイヤーのボム持ち状態を取得する
	PlayerMovement* GetPlayerMovement() const { return m_playerMovement.get(); }
	// プレイヤーのボム持ち状態を取得する
	PlayerBomHand* GetPlayerBomHand() const { return m_playerBomHand.get(); }

public:
	// コンストラクタ
	PlayerState(Camera* camera, const std::vector<std::unique_ptr<Wall>>& wall, DirectX::SimpleMath::Vector3 pos, std::vector<std::shared_ptr<Floor>>& floors);
	//デストラクタ
	~PlayerState();
	//初期化する
	void Initialize(CommonResources* resources);
	//新しい状態に遷移する
	void ChangeState(IPlayerState* newPlayerState);
	//更新する
	void Update(const float& elapsedTime);
	//描画する
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection);
	//後処理を行う
	void Finalize();
	//敵と触れた時の効果
	void EnemyHitFlashing(const DirectX::Model& model, const DirectX::FXMMATRIX world, const  DirectX::CXMMATRIX view, const  DirectX::CXMMATRIX projection);
	//敵と当たった時
	void EnemyHit(const float& elapsedTime);

private:
	//床との当たり判定
	void CheckCollisionWithFloor();
	//壁との当たり判定
	void CheckCollisionWithWall();
	// キー関係
	void CheckKeyboard();
	//事前更新
	void PreUpdate();
	//事後更新
	void PostUpdate();
	//影の初期化
	void InitializeShadow();
	// ボムの初期化
	void InitializeBom();
	// プレイヤーの状態の初期化
	void InitializeStates();
	//　デバッグコリジョンレンダーの初期化
	void InitializeCollisionDebugRenderer();
	//プレーヤーと爆弾の判定
	void CheckBom();
	//爆弾を投げたか投げてないか
	bool IsThroeableBom();

public:
	//ボムの最大数
	static constexpr int BOMMAX = 3;
	// 速さ
	static constexpr float SPEED = 0.15f;
	// 重力
	static constexpr float GRVITY = 2.5f;
	// プレイヤーモデルの大きさ
	static constexpr float PLAYERMODLSCALE = 0.008;

private:
	// 共通リソース
	CommonResources* m_commonResources;
	// 床
	std::vector<std::shared_ptr<Floor>> m_floors;
	// コリジョンデバックレンダー
	std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;
	//壁
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	//カメラ
	Camera* m_camera;
	// 影オブジェクト
	std::unique_ptr<Shadow> m_shadow;
	//現在の状態
	IPlayerState* m_currentState;
	// Playerのモデル
	std::unique_ptr<DirectX::Model> m_playerModel;
	//ボム
	std::vector<std::unique_ptr<BomState>> m_bom;
	// クォータニオン：モデルの回転を制御する
	DirectX::SimpleMath::Quaternion m_rotate;
	// 角度
	DirectX::SimpleMath::Vector3 m_angle;
	// バウディングボックスのスケール
	DirectX::SimpleMath::Vector3 m_extentsScale;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// 注視点
	DirectX::SimpleMath::Vector3 m_target;
	//戻り値の値の格納
	DirectX::SimpleMath::Vector3 m_pushBack;
	//バウディングボックス
	DirectX::BoundingBox m_boundingBox;
	// ボムが投げられたかどうかを管理するフラグ
	bool m_bomExist[BOMMAX];
	//生存しているか
	bool m_exist;
	// 壁のヒットフラグ
	bool m_isHitWall;
	// 床のヒットフラグ
	bool m_isHitFloor;
	//敵とのヒット
	bool m_isEnemyHit;
	// 衝突時のタイマー
	float m_hitTimer;
	// 点滅状態
	bool m_isFlashing;
	// 点滅のためのタイマー
	float m_flashTimer;
	//持っているボムのインデックス
	int m_index;
	//一度ボムを投げたか投げてないか
	bool m_isThrow;
	// プレイヤアイドリング
	std::unique_ptr<PlayerIdling> m_playerIdling;
	// プレイヤースボム持ち
	std::unique_ptr<PlayerBomHand> m_playerBomHand;
	//プレイヤーの動き
	std::unique_ptr< PlayerMovement> m_playerMovement;
};
#endif		// PLAYER_DEFINED