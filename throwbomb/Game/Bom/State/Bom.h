/*
  @file bom.h
  @brief ボムクラス
*/
#pragma once
#ifndef BOM_H
#define BOM_H

#include "Game/Scene/PlayScene.h"
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Effects.h>
#include <GeometricPrimitive.h>

// 前方宣言
class CommonResources;
class PlayScene;
class Wall;
class PlayerState;

class Bom
{
	public:
	// 角度を取得する
	float GetAngle() const { return m_currentAngle; }
	// 角度を設定する
	//void SetAngle(const float& angle) { m_angle = angle; }
	// スケールを取得する
	float GetScale() const { return m_scale; }
	// スケールを設定する
	void SetScale(const float& scale) { m_scale = scale; }
	// 位置を取得する
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 位置を設定する
	void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
	// プレイシーンを取得する
	PlayScene* GetPlayScene() { return m_playScene; }
	//爆弾の生成設定
	void SetExist(DirectX::SimpleMath::Vector3 playerPosition, const DirectX::SimpleMath::Vector3& playerForwardDirection);
	void SetExistStraight(DirectX::SimpleMath::Vector3 playerPosition, const DirectX::SimpleMath::Vector3& playerForwardDirection);
	//生存の獲得
	bool GetExist() {return m_exist; }
	bool GetMove() { return m_isMove; }
	bool GetExploded() { return m_exploded; }
	// バウンディングスフィアを取得する
	DirectX::BoundingSphere GetBoundingSphere() const { return m_BoundingSphere; }
public:
	// コンストラクタ
	 Bom(PlayScene* playScene, Graphics* graphics, const std::vector<std::unique_ptr<Wall>>& wall) ;
	//デストラクタ
	~Bom();
	//初期化する
	void Initialize(CommonResources* resources);
	//更新する
	void Update(const float& elapsedTime);
	//描画する
	void Render(DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix projection) ;
	//境界の球の生成
	void DrawTurretBoundingSphere();
	//後処理を行う
	void Finalize();
	//爆弾の挙動
	void Movement(const float& elapsedTime);
	//爆弾の境界線を表示
	void DrawExplosionBoundingSphere();
	//爆発の挙動
	void Explode(const float& elapsedTime);
	//爆弾の角度を変える
	void BomScaleChange(const float& elapsedTime, const DirectX::SimpleMath::Vector3& playerPosition);
	//当たり判定
	void CheckHit(DirectX::BoundingBox boundingBox);
	//爆弾を持つ
	void Hold(DirectX::SimpleMath::Vector3 playerPosition);
	//バウンドする
	void HandleCollision(const float& elapsedTime);

private:
	// グラフィックス
	Graphics* m_graphics;
	// 共通リソース
	CommonResources* m_commonResources;
	// デバイスコンテキスト
	ID3D11DeviceContext* m_context;
	// プレイシーン
	PlayScene* m_playScene;
	// ジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;
	//爆発
	std::unique_ptr<DirectX::GeometricPrimitive> m_explosionSphere;
	// バウンディングスフィア
	DirectX::BoundingSphere m_BoundingSphere;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_primitiveBatch;
	std::unique_ptr<DirectX::BasicEffect> m_basicEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//壁
	const std::vector<std::unique_ptr<Wall>>& m_wall;
private:
	// 質量
	float m_mass;
	// 角度
	int m_angle;
	// スケール
	float m_scale;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	//速度
	DirectX::SimpleMath::Vector3 m_velocity;
	//加速度
	DirectX::SimpleMath::Vector3 m_gravity;
	//カウント
	float m_cnt;
	//爆発までの時間
	float m_cntTime;
	//反発係数
	float m_restitution;
	// 速さ
	static constexpr float SPEED = 0.05f;
	//生存
	bool m_exist;
	// 爆発までの時間
	float m_timeToExplode;  
	// 爆発したかどうか
	bool m_exploded; 
	//放ってる
	bool m_isMove = false;
	//バウンドしてか
	bool m_isBounce;
	// バウンドの反発係数
	float m_bounceFactor;  
	//爆発の時間
	float m_explosionTime;  
	//現在の角度
	float m_currentAngle;
	//角度変更速度
	float m_angleSpeed = 45;
	// 爆発のスフィアコライダー
	std::unique_ptr<DirectX::BoundingSphere> m_Sphere; 

};
#endif		// Bom_DEFINED