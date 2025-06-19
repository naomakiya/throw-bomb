/*
  @file BomExist.h
  @brief ボム生存クラス
*/
#pragma once
#ifndef BOMEXIST_DEFINED
#define BOMEXIST_DEFINED

#include"Interface/IBomState.h"

class CommonResources;
class BomState;
class Wall;
class PlayerState;

class BomExist :public IBomState
{
public:
	// コンストラクタ
	BomExist(BomState* bomState, PlayerState* player);
	//デストラクタ
	~BomExist();
	//初期化する
	void Initialize(CommonResources* resources)override;
	// 事前更新する
	void PreUpdate();
	//更新する
	void Update(const float& elapsedTime) override;
	// 事後更新する
	void PostUpdate();
	//描画する
	void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection) override;
	//後処理を行う
	void Finalize() override;
private:
	// 共通リソース
	CommonResources* m_commonResources;
	// ボム状態
	BomState* m_bom;
	// プレイヤ-
	PlayerState* m_player;

	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// モデル
	std::unique_ptr<DirectX::Model> m_bomModel;

	//現在の投げる角度
	float m_currentAngle;
	//押し時間
	float m_holdStartTime;
	// プレイヤーの上にスポーンするボムのY軸オフセット
	static constexpr float BomSpawnHeight = 1.5f;

	// 生存の有無
	bool m_isExist;
	// spaceを押しているか
	bool m_isSpaceHeld;
	// 角度が増加中
	bool m_isAngleIncreasing;
	//ボムを持っているか
	bool m_isHoldingBom;
	
	//角度の速度
    const float m_angleSpeed = 15.0f;
};
#endif		// BOMEXIST_DEFINED