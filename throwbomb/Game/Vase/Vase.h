/*
  @file  Vase.h
  @brief 壺クラス
*/

#pragma once
#ifndef VASE_DEFINED
#define VASE_DEFINED

// 前方宣言
class CommonResources;
class CollisionDebugRenderer;

class Vase
{
public:
	// 位置を取得する
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 壺の生存取得
	bool GetExist() const { return m_isExist; }
	// 壺の生存設定
	void SetExist(const bool exist) { m_isExist = exist; }
	// バウンディングボックスを取得する
	DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
	// バウンディングスフィアを取得する
	DirectX::BoundingSphere GetBoundingSphere() const { return m_boundingSphere; }

public:
	// コンストラクタ
	Vase(DirectX::SimpleMath::Vector3 pos);
	//デストラクタ
	~Vase();
	//初期化する
	void Initialize(CommonResources* resources);
	//描画する
	void Render(ID3D11DeviceContext* context,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection) ;
	//後処理を行う
	void Finalize();
	//壺破壊後の処理
	void BreacVase();

private:
	// 共通リソース
	CommonResources* m_commonResources;
	// 壺モデル
	std::unique_ptr<DirectX::Model> m_vaseModel;
	// デバッグ当たり判定の描画
	std::unique_ptr<CollisionDebugRenderer>  m_collisionDebugRenderer;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// バウンディングボックス
	DirectX::BoundingBox m_boundingBox;
	// バウンディングスフィア
	DirectX::BoundingSphere m_boundingSphere;
	//生存
	bool m_isExist;
	
};
#endif		// PLAYER_DEFINED