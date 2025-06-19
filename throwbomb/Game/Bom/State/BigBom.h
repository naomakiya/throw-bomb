/*
  @file bom.h
  @brief ボムクラス
*/
#pragma once
#ifndef BIGBOM_DEFINED
#define BIGBOM_DEFINED
#include"Game/Scene/PlayScene.h"
#include <GeometricPrimitive.h>
#include <Game/Player/State/PlayerIdling.h>


// 前方宣言
class CommonResources;
class PlayScene;

class BigBom
{
	public:
	// 角度を取得する
	//float GetAngle() const { return m_angle; }
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
	void SetExist(DirectX::SimpleMath::Vector3 playerPosition);
	//生存の獲得
	bool GetExist() {return m_exist; }
	// バウンディングスフィアを取得する
	DirectX::BoundingSphere GetBoundingSphere() const { return m_BoundingSphere; }
public:
	// コンストラクタ
	 BigBom(PlayScene* playScene, Graphics* graphics, const std::vector<std::unique_ptr<Wall>>& wall) ;
	//デストラクタ
	~BigBom();
	//初期化する
	void Initialize(CommonResources* resources);
	//境界球
	DirectX::BoundingSphere CreateBoundingSphere(const float& radius);
	//更新する
	void Update(const float& elapsedTime);
	//描画する
	void Render(DirectX::SimpleMath::Matrix view,
		DirectX::SimpleMath::Matrix projection) ;
	//境界の球の生成
	void DrawTurretBoundingSphere();
	//後処理を行う
	void Finalize();
	//
	void DrawExplosionBoundingSphere();
	//爆発の挙動
	void Explode(const float& elapsedTime);

	void CheckHit(DirectX::BoundingBox boundingBox);
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
	//壁
	const std::vector<std::unique_ptr<Wall>>& m_wall;
	// 質量
	float m_mass;
	// スケール
	static float m_scale;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	//加速度
	DirectX::SimpleMath::Vector3 m_gravity;
	//カウント
	float m_cnt;
	//爆発までの時間
	float m_cntTime;
	// 速さ
	static constexpr float SPEED = 0.05f;
	//生存
	bool m_exist;
	// 爆発したかどうか
	bool m_exploded; 
	//放ってる
	bool m_isMove = false;
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