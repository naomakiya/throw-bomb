/*
    @file	EnemyManager.cpp
    @brief	敵をまとめるクラス
*/
#include"pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game/Factory/EnemyFactory.h"
#include "Game/Factory/StraighteningEnemyFactory.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/BomState.h"
#include "Game/Collision/Collision.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Libraries/MyLib/CollisionMesh.h"

void EnemyManager::SetVacePos(DirectX::SimpleMath::Vector3 vasePos)
{
    m_vacePos = vasePos;
    for (auto& enemy : m_enemy)
    {
        float distance = (enemy->GetPosition() - vasePos).Length();
        // 15以内にいる敵にだけ適用
        if (distance <= 15.0f) 
        {
            enemy->SetPointPosition(vasePos);
        }
    }
}

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemyManager::EnemyManager(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, std::vector<DirectX::SimpleMath::Vector3> enemyMap,const std::vector<DirectX::SimpleMath::Vector3> patrolPath, std::vector<DirectX::SimpleMath::Vector3> straighteningEnemyMap,
    std::vector<std::unique_ptr<mylib::CollisionMesh>>& collisionMeshes)
    :
    m_position{},
    m_commonResources{},
    m_enemy{},
    m_straighteningEnemy{},
    m_boundingBox{},
    m_isHit{},
    m_wall{wall},
    m_player{player},
    m_enemyMap{ enemyMap },
    m_patrolPath{ patrolPath },
    m_exist{true},
    m_scale{},
    m_straighteningEnemyMap{ straighteningEnemyMap },
    m_enemydashuModel{},
    m_empty{},
    m_collisionMeshes{ collisionMeshes },
    m_isAllStraighteningEnemiesDefeated{ false },
    m_isAllEnemiesDefeated{ false }
{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
EnemyManager::~EnemyManager()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void EnemyManager::Initialize(CommonResources* resources)
{
    assert(resources);
   
    m_commonResources = resources;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    //敵のモデルの読み込み準備
    std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
    fx->SetDirectory(L"Resources/Models/Enemy");

    //敵の読み取り
    m_enemyModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("Enemy").c_str(), *fx);
    m_enemydashuModel = DirectX::Model::CreateFromCMO(device, ResourceManager::getModelPath("DashuEnemy").c_str(), *fx);
    // 敵を生成する
    m_enemy = EnemyFactory::CreateEnemy(m_wall, m_player, m_commonResources, m_enemyMap, m_patrolPath);
    //突進する敵を生成
    m_straighteningEnemy = StraighteningEnemyFactory::CreateEnemy(m_wall, m_player, m_commonResources, m_straighteningEnemyMap, m_patrolPath);

}
//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void EnemyManager::Update(const float& elapsedTime)
{
    //敵
    EnemyUpdate(elapsedTime);
   //突進する敵
    StraighteningEnemyUpdate(elapsedTime);

    //全ての敵がいないなら敵が無い無い状態をオンにする
    if (m_isAllEnemiesDefeated && m_isAllStraighteningEnemiesDefeated) {
        m_empty = true;
    }
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void EnemyManager::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection)
{
    //追跡エネミー
    for (auto& enemy : m_enemy)
    {
        //敵が生きているか生きていない確認
        if (enemy->GetExist())enemy->Render(context, view, projection, *m_enemyModel);
    }
    //　突進する敵
    for (auto& straighteningEnemy : m_straighteningEnemy)
    {
        //生きているなら
        if (straighteningEnemy->GetExist()) straighteningEnemy->Render(context, view, projection, *m_enemydashuModel);
    }
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void EnemyManager::Finalize()
{
    m_enemy.clear();
    m_straighteningEnemy.clear();
}

//---------------------------------------------------------
// 敵の更新する
//---------------------------------------------------------
void EnemyManager::EnemyUpdate(const float& elapsedTime)
{
    m_isAllEnemiesDefeated = true; // すべての敵が倒されたか判定するフラグ
    //更新
    for (auto& enemy : m_enemy)
    {
        //生きているなら
        if (enemy->GetExist())
        {
            enemy->Update(elapsedTime);
            m_isAllEnemiesDefeated = false; // 一体でも生存しているなら false にする
        }
    }

    //当たり判定
    for (auto& enemy : m_enemy)
    {
        CollisionMeshDeterminationEnemy(enemy);
  
        // 敵が生きていないならスキップ
        if (!enemy->GetExist()) continue;

        for (int i = 0; i < 3; i++)
        {
            //ボムが表示されているなら
            if (m_player->GetBomState(i)->GetExist())
            {
                //敵とボムが当たったなら
                if (mylib::Collision::BoundingCheckCollision(enemy->GetBoundingBox(), m_player->GetBomState(i)->GetBoundingSphere()))
                {   // 爆発状態なら
                    if (m_player->GetBomState(i)->GetBomPresent() == BomState::EXPLOSION) enemy->HPDown();
                }
            }
        }

        // プレイヤが無敵でないなら
        if (!m_player->GetHitEnemy())
        {
            //プレイヤーと敵の当たり判定
            if (mylib::Collision::BoundingCheckBoxCollision(m_player->GetBoundingBox(), enemy->GetBoundingBox())) m_player->SetHitEnemy(true);
            
        }
    }
  
}

//---------------------------------------------------------
// 突進敵の更新する
//---------------------------------------------------------
void EnemyManager::StraighteningEnemyUpdate(const float& elapsedTime)
{
    m_isAllStraighteningEnemiesDefeated = true;

    // 突進敵の更新
    for (auto& straighteningEnemy : m_straighteningEnemy)
    {
        //生きているなら
        if (straighteningEnemy->GetExist()) {
            straighteningEnemy->Update(elapsedTime);
            m_isAllStraighteningEnemiesDefeated = false;
        }
    }

    //敵の当たり判定
    for (auto& enemy : m_straighteningEnemy)
    {
        // 敵が生きていないならスキップ
        if (!enemy->GetExist()) continue;

        CollisionMeshDeterminationEnemy(move(enemy));

        for (int i = 0; i < 3; i++)
        {
            //ボムが描画されているなら
            if (m_player->GetBomState(i)->GetExist())
            {   //敵とボムが当たったなら
                if (mylib::Collision::BoundingCheckCollision(enemy->GetBoundingBox(), m_player->GetBomState(i)->GetBoundingSphere()))
                {   // 爆発状態なら
                    if (m_player->GetBomState(i)->GetBomPresent() == BomState::EXPLOSION) enemy->HPDown();
                }
            }
        }

        //敵とプレイヤーが生きているなら
        if (!m_player->GetHitEnemy())
        {
            //プレイヤーと敵の当たり判定
            if (mylib::Collision::BoundingCheckBoxCollision(m_player->GetBoundingBox(), enemy->GetBoundingBox()))
            {
                m_player->SetHitEnemy(true);
            }
        }
    }
}

//---------------------------------------------------------
// 坂の当たり判定(敵）
//---------------------------------------------------------
void EnemyManager::CollisionMeshDeterminationEnemy(const std::unique_ptr<IEnemyState>& enemy)
{
    using namespace DirectX::SimpleMath;

    //コリジョンメッシュの当たり判定情報の取得
    mylib::Collision::CollisionResult collision = mylib::Collision::CheckCollisionMesh(m_collisionMeshes, enemy->GetPosition(), m_player->GetForwardDirection());

    // 衝突が発生した場合の処理
    if (!collision.hasHit) return;

    collision.normal.Normalize();

    // 敵の位置と衝突点のY座標を比較
    float enemyYPosition = enemy->GetPosition().y;
    float collisionMeshYPosition = collision.hitPosition.y;

    // 敵がコリジョンメッシュより上にいない場合
    if (enemyYPosition <= collisionMeshYPosition)
    {
        // 強く押し返す場合
        if (collisionMeshYPosition - enemyYPosition > 0.25f)
        {
            float pushBackStrength = 0.5f;
            Vector3 pushBackPosition = collision.hitPosition + collision.normal * pushBackStrength;


            enemy->SetPosition(pushBackPosition);
        }
        else
        {
            // 坂道を登れるように少しだけ押し返す
            float smallPushBackStrength = 0.01f;
            Vector3 pushBackPosition = collision.hitPosition + collision.normal * smallPushBackStrength;

            enemy->SetPosition(pushBackPosition);

        }
    }
}