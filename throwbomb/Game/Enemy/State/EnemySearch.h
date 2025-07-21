/*
  @file  EnemySearch.h
  @brief 探している状態クラス
*/
#pragma once
#ifndef ENEMYTSEARCH_DEFINED
#define ENEMYTSEARCH_DEFINED
#include "Interface/IEnemyState.h"

// 前方宣言
class CommonResources;
class EnemyState;
class Wall;
class PlayerState;

class EnemySearch : public IEnemyState
{
public:
    // コンストラクタ
    EnemySearch(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
    // デストラクタ
    ~EnemySearch();
    // 初期化
    void Initialize(CommonResources* resources);
    // 前更新
    void PreUpdate();
    //更新
    void Update(const float& elapsedTime);
    //事後更新
    void PostUpdate();
    //描画
    void Render(ID3D11DeviceContext* context, DirectX::CommonStates* states, 
        const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection,
        const DirectX::Model& model);
    //処理
    void Finalize();
    // 壁との衝突処理
    DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& wallBox, const DirectX::BoundingBox& entityBox);
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
    //HPを減らす
    void HPDown() {};
    // 現在の向いている方向を取得する
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
private:
    // 共通リソース
    CommonResources* m_commonResources;
    // 敵の状態管理オブジェクト
    EnemyState* m_enemy;
    // プレイヤー
    PlayerState* m_player;
    // 壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
     // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotate;
    // 敵のバウンディングボックス
    DirectX::BoundingBox m_boundingBox;
    // 敵のバウンディングスフィア
    DirectX::BoundingSphere m_boundingSphere;
    // 探索用バウンディングボックス
    DirectX::BoundingOrientedBox m_searchBoundingBox;

    // Texture用メンバー（テクスチャーとスプライトバッチ）
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_mysteryTexture;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    //テクスチャー角度
    float m_textureAngle;
    //角度の合計
    float m_totalTime;
    // ワールド行列
    DirectX::SimpleMath::Matrix m_worldMatrix;
    // 敵の現在位置
    DirectX::SimpleMath::Vector3 m_position;
    // 目標地点のインデックス
    unsigned int m_currentGoalNo;
    // 経過時間
    float m_time;
    // 敵のスケール
    float m_scale;
    //モデルの大きさ
    float m_modelScale;
    // 敵が生存しているか
    bool m_exist;
};
#endif  //ENEMYTRACKING_DEFINED