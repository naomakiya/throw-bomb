#pragma once
#ifndef IENEMYSTATE_DEFINED
#define IENEMYSTATE_DEFINED
#include "pch.h"
// IEnemyインターフェースを定義する
// 
// 前方宣言
class CommonResources;

class IEnemyState
{
public:
	// スケールを取得する
	virtual float GetScale() const = 0;
	// 位置を取得する
	virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;
	// 位置を設定する
	virtual  void SetPosition(const DirectX::SimpleMath::Vector3& position) = 0;
	// 目的地の位置を取得
	virtual void SetPointPosition(DirectX::SimpleMath::Vector3& position) = 0;
	//敵の生存取得
	virtual  bool GetExist() const = 0;
	//敵の生存設定
	virtual  void SetExist(const bool exist) = 0;
	// バウンディングボックスを取得する
	virtual DirectX::BoundingBox GetBoundingBox() const = 0;
	// 現在の向いている方向を取得する
	virtual DirectX::SimpleMath::Vector3 GetForwardDirection() const = 0;
public:
	// 仮想デストラクタ
	virtual ~IEnemyState() = default;
	// 初期化する
	virtual void Initialize(CommonResources* resources,
		DirectX::SimpleMath::Vector3 pos) = 0;
	//事前更新
	virtual void PreUpdate() = 0;
	// 更新する
	virtual void Update(const float& elapsedTime) = 0;
	//事後更新
	virtual void PostUpdate() = 0;
	// 描画する
	virtual void Render(ID3D11DeviceContext* context,
		                const DirectX::SimpleMath::Matrix& view,
		                const DirectX::SimpleMath::Matrix& projection,
						const DirectX::Model& model) = 0;
	// 後処理を行う
	virtual void Finalize() = 0;
	//HPを減らす
	virtual void HPDown() = 0;
};
#endif