/*
  @file  Wall.h
  @brief 壁クラス
*/

#pragma once
#ifndef WALL_DEFINED
#define WALL_DEFINED

// 前方宣言
class CommonResources;
class CollisionDebugRenderer;

class Wall
{
public:
	//壁の種類
	enum class WallType {
		NONE = 0,			 //	存在しない
		Indestructible = 1,  // 破壊不可能な壁
		Destructible = 2     // 破壊可能な壁
	};

public:
	// スケールを取得する
	float GetScale() const { return m_scale; }
	// スケールを設定する
	void SetScale(const float scale) { m_scale = scale; }
	// 位置を取得する
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// 位置を設定する
	void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
	//生存取得
	bool GetExist() const { return m_exist; }
	//生存設定
	void SetExist(const bool exist) { m_exist = exist; }
	// バウンディングボックスを取得する
	DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
	//壁の種類の確認
	WallType GetWallType() const { return m_wallType; }
	// 可視状態を取得
	bool IsVisible() const { return m_isVisible; }
	// 可視状態を設定
	void SetVisible(bool visible) { m_isVisible = visible; }

public:
	// コンストラクタ
	Wall(WallType wallType, std::shared_ptr<DirectX::Model> model,const DirectX::SimpleMath::Vector3& position);
	//デストラクタ
	~Wall();
	//初期化する
	void Initialize(CommonResources* resources);
	//描画する
	void Render(ID3D11DeviceContext* context,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection);
	//後処理を行う
	void Finalize();
	//壁のバウンディングボックス同士の当たり判定
	DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& a, const DirectX::BoundingBox& b);

private:
      // 共通リソース
      CommonResources* m_commonResources;  
      // デバッグ描画
      std::unique_ptr<CollisionDebugRenderer>  m_collisionDebugRenderer;  
	  // 深度ステンシルステート
	  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	  // テクスチャリソースビュー
	  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;  
	  // 壁の種類
      WallType m_wallType; 
	  // 壁モデル
	  std::shared_ptr<DirectX::Model> m_wallModel;
	  // バウンディングボックス（当たり判定用）
      DirectX::BoundingBox m_boundingBox;  
	  // 壁の位置
	  DirectX::SimpleMath::Vector3 m_position;
	  // スケール倍率
	  float m_scale;
	  //モデルスケール
	  float m_modelScale;
	   // 生存フラグ
      bool m_exist; 
	  // 可視フラグ
      bool m_isVisible; 
	  // 衝突の閾値（めり込み量がこれ未満なら無視）
	  static float THRESHOLD;
};
#endif		// PLAYER_DEFINED