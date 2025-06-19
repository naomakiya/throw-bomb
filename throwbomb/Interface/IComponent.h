#pragma once
#ifndef ICOMPONENT_DEFINED
#define ICOMPONENT_DEFINED
#include <SimpleMath.h>
#include <Model.h>
#include <Framework/CommonResources.h>

// IComponentインターフェースを定義する
class IComponent
{
public:
	// ノード番号を取得する
	virtual int GetNodeNumber() const = 0;
	// 部品IDを取得する
	virtual int GetPartID() const = 0;
	// 部品番号を取得する
	virtual int GetPartNumber() const = 0;
	// 親を取得する
	virtual IComponent* GetParent() const = 0;
	// 現在の位置を取得する
	virtual DirectX::SimpleMath::Vector3 GetPosition() const = 0;
	// 現在の位置を設定する
	virtual void SetPosition(const DirectX::SimpleMath::Vector3& curretPosition) = 0;
	// モデルを取得する
	virtual DirectX::Model* GetModel() = 0;

public:
	// 仮想デストラクタ
	virtual ~IComponent() = default;
	// 初期化する
	virtual void Initialize() = 0;
	// 更新する
	virtual void Update(const DirectX::SimpleMath::Vector3& position) = 0;
	// 描画する
	virtual void Render() = 0;
	// 後処理を行う
	virtual void Finalize() = 0;
};

#endif		// ICOMPONENT_DEFINED
