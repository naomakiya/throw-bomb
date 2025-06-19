#pragma once
#ifndef STRAIGHTENINGENEMYSTATE_DEFINED
#define STRAIGHTENINGENEMYSTATE_DEFINED
#include"Interface/IEnemyState.h"


// 前方宣言
class CommonResources;
class EnemyScene;
class PlayerState;
class StraighteningEnemyExist;
class StraighteningEnemySearch;
class StraighteningEnemyDash;
//class EnemyStop;
class Shadow;
class Wall;
class CollisionDebugRenderer;

class StraighteningEnemyState : public IEnemyState
{
public:
    // スケールを取得する
    float GetScale() const { return m_scale; }
    // 位置を取得する
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector3& position){ m_position = position; }
    // 目的地の位置を設定
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //敵の生存取得
    bool GetExist() const { return isExist; }
    //敵の生存設定
    void SetExist(const bool exist) { isExist = exist; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // バウンディングボックスを設定する
    void SetBoundingBox(const DirectX::BoundingBox box)  { m_boundingBox = box; }
    // バウンディングスフィアを取得する
    DirectX::BoundingSphere GetBoundingSphere() const { return m_boudingSphere; }
    // バウンディングスフィアを設定する
    void SetBoundingSphere(const DirectX::BoundingSphere sphere) { m_boudingSphere = sphere; }
    //// モデルを取得する
    DirectX::Model* GetModel()  { return m_enemyModel.get(); }
    // 現在の向いている方向を取得する
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
    // プレイヤーステートを取得する
    PlayerState* GetPlayer() { return m_player; }

    // 敵を取得する
    StraighteningEnemyExist* GetStraighteningEnemyExist() const { return m_exist.get(); }
    // 探索状態を取得する
    StraighteningEnemySearch* GetStraighteningEnemySearch() const { return m_search.get(); }
    //敵のダッシュを取得する
    StraighteningEnemyDash* GetStraighteningEnemyDash() const {return m_dash.get();}
    ////敵の止まるを取得する
    //EnemyStop* GetEnemyStop() const { return m_stop.get(); }
public:
    // コンストラクタ
    StraighteningEnemyState(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
    // デストラクタ
    ~StraighteningEnemyState();
    // 初期化する
    void Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos);
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
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection,
        const DirectX::Model& model) ;
    // 後処理を行う
    void Finalize() ;
    //壁との押し出し判定
    DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& wallBox);
    // 敵同士の当たり判定を行う
    //void CheckEnemyCollisions(std::vector<std::unique_ptr<StraighteningEnemyState>>& enemies);
    //エイチピーを減らす
    void HPDown();
private:
    //プレイヤー
    PlayerState* m_player;
    // 共通リソース
    CommonResources* m_commonResources;
    //当たり判定の描画
    std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;
    //状態
    IEnemyState* m_currentState;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemyのモデル
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
    //バウディングスフィア
    DirectX::BoundingSphere m_boudingSphere;
    //敵の経路パス
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
    // 影オブジェクト
    std::unique_ptr<Shadow> m_shadow;
    //当たり判定
    //rcPolyMesh* m_polyMesh;
    // 質量
    float m_mass;
    // 角度
    float m_angle;
    // スケール
    float m_scale;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotate;
    // 影の位置
    DirectX::SimpleMath::Vector3 m_shadowPosition;
    // 生存
    bool isExist;
    //HP
    int m_hart;
    //生存している
    std::unique_ptr<StraighteningEnemyExist> m_exist;
    //探索している
    std::unique_ptr<StraighteningEnemySearch> m_search;
    //走っている
    std::unique_ptr<StraighteningEnemyDash> m_dash;
    ////止まっているている
    //std::unique_ptr<EnemyStop> m_stop;
};
#endif // ENEMY_DEFINED