/*
  @file　EnemyState.h
  @brief 敵の状態クラス
*/
#pragma once
#ifndef ENEMYSTATE_DEFINED
#define ENEMYSTATE_DEFINED
#include"Interface/IEnemyState.h"

// 前方宣言
class CommonResources;
class EnemyScene;
class PlayerState;
class EnemyExist;
class EnemyMovement;
class EnemySearch;
class EnemyTracking;
class EnemyStop;
class EnemyPointMovement;
class CollisionDebugRenderer;
class Shadow;
class Wall;


class EnemyState : public IEnemyState
{
public: //interface側での作成
    // 位置を取得する
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector3& position){ m_position = position; }
    // 目的地の位置を設定
    void SetPointPosition(DirectX::SimpleMath::Vector3& position);
    //目的地の取得
    DirectX::SimpleMath::Vector3 GetPointPosition() const { return m_pointPosition; }
    //敵の生存取得
    bool GetExist() const { return isExist; }
    //敵の生存設定
    void SetExist(const bool exist) { isExist = exist; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // 現在の向いている方向を取得する
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
public:
    static constexpr float ENEMYMODELSCALE = 0.006f;
public: //このクラスでの作成
    // バウンディングボックスを設定する
    void SetBoundingBox(const DirectX::BoundingBox box)  { m_boundingBox = box; }
    // バウンディングスフィアを取得する
    DirectX::BoundingSphere GetBoundingSphere() const { return m_boudingSphere; }
    // バウンディングスフィアを設定する
    void SetBoundingSphere(const DirectX::BoundingSphere sphere) { m_boudingSphere = sphere; }
    // プレイヤーステートを取得する
    PlayerState* GetPlayer() const { return m_player; }
    // 敵アイドリングを取得する
    EnemyExist* GetEnemyExist() const { return m_exist.get(); }
    // 敵ストライクフォワーディングを取得する
    EnemyMovement* GetEnemyMovement() const { return m_movement.get(); }
    // 敵の追跡モードを取得する
    EnemyTracking* GetEnemyTracking() const {return m_tracking.get();}
    // 敵の止まるを取得する
    EnemyStop* GetEnemyStop() const { return m_stop.get(); }
    // プレイヤを探す
    EnemySearch* GetEnemySearch() const { return m_search.get();}

public:
    // コンストラクタ
    EnemyState(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath,const DirectX::SimpleMath::Vector3& position);
    // デストラクタ
    ~EnemyState();
    // 初期化する
    void Initialize(CommonResources* resources);
    // 新しい状態に遷移する
    void ChangeState(IEnemyState* newState);
    // 事前更新
    void PreUpdate() ;
    // 更新する
    void Update(const float& elapsedTime) ;
    // 事後更新
    void PostUpdate() ;
    // 描画する
    void Render(ID3D11DeviceContext* context,
        DirectX::CommonStates* states,
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection,
        const DirectX::Model& model) ;
    // 後処理を行う
    void Finalize() ;
    //エイチピーを減らす
    void HPDown();

private:
    // 共通リソース
    CommonResources* m_commonResources;
    // プレイヤー
    PlayerState* m_player;
    // デバックコリジョン
    std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;
    // 状態
    IEnemyState* m_currentState;
    // 壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemyのモデル
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
    //バウディングスフィア
    DirectX::BoundingSphere m_boudingSphere;
    // 敵の経路パス
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
    // 影オブジェクト
    std::unique_ptr<Shadow> m_shadow;
    // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotate;
    // 総時間
    float m_totalSeconds;
    // 角度
    float m_angle;
    // スケール
    float m_scale;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    // 目的の位置
    DirectX::SimpleMath::Vector3 m_pointPosition;
    // 生存
    bool isExist;
    // HP
    int m_hart;
    // 生存している
    std::unique_ptr<EnemyExist> m_exist;
    // 動いている
    std::unique_ptr<EnemyMovement> m_movement;
    // 目的地へ移動
    std::unique_ptr<EnemyPointMovement> m_pointMovement;
    // 追跡している
    std::unique_ptr<EnemyTracking> m_tracking;
    // 止まっているている
    std::unique_ptr<EnemyStop> m_stop;
    // 探している 
    std::unique_ptr<EnemySearch> m_search;

    // 巡回経路
    std::vector<DirectX::SimpleMath::Vector3> patrolPath; 
};
#endif // ENEMY_DEFINED