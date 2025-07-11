/*
  @file  BomState.h
  @brief ボム状態クラス
*/

#pragma once
#ifndef BOMSTATE_DEFINED
#define BOMSTATE_DEFINED
#include"Interface/IBomState.h"

// 前方宣言
class CommonResources;
class BomExist;
class BomMovement;
class BomExplosion;
class BomStay;
class Wall;
class PlayerState;

class BomState : public IBomState
{

public:
	// 重力
	static constexpr float GRAVITY = -9.81f;
	// ボムモデルの大きさ
	static constexpr float BOMBMODELSIZE = 0.005f;
	// 当たり判定の大きさ
	static constexpr float BOUNDINGSPHERERADIUS = 0.5f;
public:
    // 角度を取得する
    float GetAngle() const { return m_currentAngle; }
	// 角度の設定
	void SetAngle(const float& angel) { m_currentAngle = angel; }
    // スケールを取得する
    float GetScale() const { return m_scale; }
    // スケールを設定する
    void SetScale(const float& scale) { m_scale = scale; }
    // 位置を取得する
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
	// ボムの状態をを設定する
	void SetBomPresent(IBomState::BomtPresent bomPresent) { m_bomPresent = bomPresent; }
	// 存在の獲得
	bool GetExist() { return m_isExist; }
	// 生存の設定
	void SetExist(const bool exist) { m_isExist = exist; }
	// 速度のゲット
	DirectX::SimpleMath::Vector3 GetVelocity() { return m_velocity; }
	void SetVelocity(DirectX::SimpleMath::Vector3 velocity) { m_velocity = velocity; }
	// バウンディングスフィアを取得する
	DirectX::BoundingSphere GetBoundingSphere() const { return m_BoundingSphere; }
	// バウンディングスフィアを設定する
	void SetBoundingSphere(DirectX::BoundingSphere boundingSphere) { m_BoundingSphere = boundingSphere; }
	// バウンド状態を取得（跳ね返り中かどうか）
	bool GetBounce() const  { return m_isBounce; } ;
	// バウンド状態を設定（跳ね返り開始/停止）
	void SetBounce(bool bounce) { m_isBounce = bounce; }
	// 爆弾の時間を設定
	void SetExplosionTimer(float time) { m_explosionTimer = time; }
	// 爆発時間を取得
	float GetExplosionTimer() const { return m_explosionTimer; }
	// ボムの状態を取得する
	IBomState::BomtPresent GetBomPresent() const { return m_bomPresent; }
	// プレイヤーを獲得
	PlayerState* GetPlayerState() { return m_player; }
    // ボムの生存を取得する
	BomExist* GetBomExist() const { return m_bomExist.get(); }
    // ボムの動き状態を取得する
	BomMovement* GetBomMovement() const { return m_bomMovement.get(); }
    // プレイヤーのボム持ち状態を取得する
	BomExplosion* GetBomExplosion() const { return m_bomExplosion.get(); }
	// ボムに待ち状態を取得
	BomStay* GetBomStay() const { return m_bomStay.get(); }

public:
	// コンストラクタ
    BomState(PlayerState* m_Player, const std::vector<std::unique_ptr<Wall>>& wall) ;
	// デストラクタ
	~BomState();
	// 初期化する
	void Initialize(CommonResources* resources) override;
	// 新しい状態に遷移する
	void ChangeState(IBomState* newBomState);
	// 事前更新
	void PreUpdate() {};
	// 更新する
	void Update(const float& elapsedTime) override;
	// 描画する
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) override;
	// 事後更新
	void PostUpdate() {};
	// 後処理を行う
	void Finalize()override;

private:
	// 共通リソース
	CommonResources* m_commonResources;
	//壁
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	//プレイヤー
	PlayerState* m_player;
	// 現在の状態クラス
	IBomState* m_currentState;
	// 現在の状態
	BomtPresent m_bomPresent;
	// 衝突判定用スフィア
	DirectX::BoundingSphere m_BoundingSphere;

	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// 速度
	DirectX::SimpleMath::Vector3 m_velocity;

	// 爆発までの残り時間
	float m_explosionTimer; 
	// スケール
	float m_scale;
	//現在の角度
	float m_currentAngle;

	// 現在の有無
	bool m_isExist;
	// 爆発済みか
	bool m_isExploded;
	// 跳ね返り中か
	bool m_isBounce;

	// ボム生存
	std::unique_ptr<BomExist> m_bomExist;
	// ボムの移動
	std::unique_ptr<BomMovement> m_bomMovement;
	// 爆発
	std::unique_ptr<BomExplosion> m_bomExplosion;
	// 置く
	std::unique_ptr<BomStay> m_bomStay;
};
#endif		// BOMSTATE_DEFINED