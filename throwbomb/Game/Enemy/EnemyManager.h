/*
    @file	EnemyManager.h
    @brief	敵をまとめるクラス
*/
#pragma once
#ifndef ENEMY_MANAGER_DEFINED
#define ENEMY_MANAGER_DEFINED

// 前方宣言
class CommonResources;
class ResourceManager;
class PlayerState;
class Wall;
class IEnemyState;
class StraighteningEnemyState;

namespace mylib
{
    class CollisionMesh;
}

class EnemyManager
{
public: 
    //敵の生存取得
    bool GetExist() const { return m_exist; }
    //敵の生存設定
    void SetExist(const bool exist) { m_exist = exist; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    //全ての敵の空の取得
    bool GetEmpty() const { return m_empty; }
    //破壊された壺の場所の取得
    void SetVacePos(DirectX::SimpleMath::Vector3 vasePos);

 public:
    // コンストラクタ
    EnemyManager(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, std::vector<DirectX::SimpleMath::Vector3> enemyMap, const std::vector<DirectX::SimpleMath::Vector3> patrolPath, std::vector<DirectX::SimpleMath::Vector3> straighteningEnemyMap, std::vector<std::unique_ptr<mylib::CollisionMesh>>& collisionMeshes);
    // デストラクタ
    ~EnemyManager();
    // 初期化する
    void Initialize(CommonResources* resources);
    // 更新する
    void Update(const float& elapsedTime);
    // 描画する
    void Render(ID3D11DeviceContext* context,
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection);
    // 後始末を行う
    void Finalize();
private:
    // 敵の更新
    void EnemyUpdate(const float& elapsedTime);
    // 突進敵の更新
    void StraighteningEnemyUpdate(const float& elapsedTime);
    // 坂の当たり判定(敵）
    void CollisionMeshDeterminationEnemy(const std::unique_ptr<IEnemyState>& enemy);
private:
    // 共通リソース
    CommonResources* m_commonResources;
    // プレイヤー
    PlayerState* m_player;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // 敵
    std::vector<std::unique_ptr<IEnemyState>> m_enemy;
    // 突進する敵
    std::vector<std::unique_ptr<IEnemyState>> m_straighteningEnemy;
    // Enemyのモデル
    std::unique_ptr<DirectX::Model> m_enemydashuModel;
    // Enemyのモデル
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
    // 敵の位置情報
    std::vector<DirectX::SimpleMath::Vector3> m_enemyMap;
    // 突進敵の位置情報
    std::vector<DirectX::SimpleMath::Vector3> m_straighteningEnemyMap;
    //探索経路
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
    // コリジョンメッシュクラス
    std::vector<std::unique_ptr<mylib::CollisionMesh>>& m_collisionMeshes;
    // スケール
    float m_scale;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    // 生存
    bool m_exist;
    // ヒットフラグ
    bool m_isHit;
    //全ての敵がいなくなった
    bool m_empty;
    //壺ポジションを取得する変数
    DirectX::SimpleMath::Vector3 m_vacePos;
};
#endif // ENEMY_MANAGER_DEFINED
