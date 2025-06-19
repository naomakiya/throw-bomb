/*
  @file Player.h
  @brief プレイヤクラス
*/
#pragma once
#ifndef  STRAIGHTENINGENEMY_SEARCH_DEFINED
#define  STRAIGHTENINGENEMY_SEARCH_DEFINED
#include "Interface/IEnemyState.h"
// 前方宣言
class CommonResources;
class StraighteningEnemyState;
class Wall;
class PlayerState;
class CollisionDebugRenderer;

class StraighteningEnemySearch :public IEnemyState
{
    // スケールを取得する
    float GetScale() const { return m_scale; }
    // 位置を取得する
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    // 目的地の位置を設定
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //敵の生存取得
    bool GetExist() const { return m_exist; }
    //敵の生存設定
    void SetExist(const bool exist) { m_exist = exist; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // バウンディングボックスを取得する
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
    // 現在の向いている方向を取得する
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
public:
     // コンストラクタ
    StraighteningEnemySearch(StraighteningEnemyState* enemyState,  
         const std::vector<std::unique_ptr<Wall>>& wall,PlayerState* player);
     // デストラクタ
     ~StraighteningEnemySearch();
     // 初期化する
     void Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos);
     // 事前更新する
     void PreUpdate();
     // 更新する
     void Update(const float&  elapsedTime);
     // 事後更新する
     void PostUpdate();
     // 描画する
     void Render(ID3D11DeviceContext* context,
         const DirectX::SimpleMath::Matrix& view,
         const DirectX::SimpleMath::Matrix& projection,
         const DirectX::Model& model);
     // 後処理を行う
     void Finalize();

     void HPDown() {};
	private:
    // 共通リソース
    CommonResources* m_commonResources;
    //敵のステート
    StraighteningEnemyState* m_enemy;
    //プレイヤー
    PlayerState* m_player;
    //当たり判定の描画
    std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemyのモデル
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
    // 探索用バウンディングボックス
    DirectX::BoundingOrientedBox m_searchBoundingBox;
	// ワールドマトリックス
	DirectX::SimpleMath::Matrix m_worldMatrix;
	// プレーヤーモデル
	DirectX::Model* m_EnemyModel;
	////モデル
	std::unique_ptr<DirectX::Model> m_model;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotate;
    //クォータオン
    DirectX::SimpleMath::Quaternion m_rotation;
    // スケール
    float m_scale;
    //生存
    bool m_exist;
    //止まる時間
    float m_stoptime;
};
#endif		// PLAYER_DEFINED