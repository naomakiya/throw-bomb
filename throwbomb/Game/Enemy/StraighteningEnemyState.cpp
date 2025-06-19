#include"pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Enemy/StraighteningEnemyState.h"
#include "Game/Enemy/StraighteningEnemyState/StraighteningEnemySearch.h"
#include "Game/Enemy/StraighteningEnemyState/StraighteningEnemyDash.h"
#include "Game/Enemy/StraighteningEnemyState/StraighteningEnemyExist.h"
#include "Game/Scene/PlayScene.h"
#include "Game/Wall/Wall.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Game/Bom/BomState.h"
#include "Recast.h"
#include "Game/Shadow/Shadow.h"
#include "Game/Collision/Collision.h"
#include "Game/Bom/State/BomStay.h"


using namespace DirectX;
using namespace DirectX::SimpleMath;

void StraighteningEnemyState::HPDown()
{
    for (int i = 0; i < 3; i++)
    {
        switch (m_player->GetBomState(i)->GetBomPresent())
        {
        case IBomState::MOVEMENT:

            break;
        case IBomState::EXPLOSION:
            m_hart -= 5;
            break;
        default:
            break;
        }
    }
}

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
StraighteningEnemyState::StraighteningEnemyState( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
    :
    m_position{},
    m_enemyModel{},
    m_commonResources{},
    m_mass{ 0 },
    m_angle{ 0 },
    m_scale{ 0.5f },
    m_currentState{},
    m_player{player},
    m_wall{wall},
    isExist{true},
    m_hart{5},
    m_boudingSphere{},
    m_collisionDebugRenderer{},
    m_shadow{},
    m_search{},
    m_shadowPosition{}
{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
StraighteningEnemyState::~StraighteningEnemyState()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void StraighteningEnemyState::Initialize(CommonResources* resources,DirectX::SimpleMath::Vector3 pos)
{
    assert(resources);
    m_commonResources = resources;
    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
   

    m_position = pos;
    m_shadowPosition = pos;

    // 影を作成する
    m_shadow = std::make_unique<Shadow>();
    m_shadow->Initialize(device, context);

    m_exist = std::make_unique<StraighteningEnemyExist>(this,m_wall);
    m_exist->Initialize(m_commonResources,pos);

    m_search = std::make_unique<StraighteningEnemySearch>(this, m_wall, m_player);
    m_search->Initialize(m_commonResources,pos);

    m_dash = std::make_unique<StraighteningEnemyDash>(this, m_wall,m_player);
    m_dash->Initialize(m_commonResources, pos);

    /* m_stop = std::make_unique<EnemyStop>(this, m_wall);
    m_stop->Initialize(m_commonResources, pos);*/

    m_boudingSphere.Center = m_position;
    m_boudingSphere.Radius = 0.5;

    m_boundingBox.Center = m_position;
    m_boundingBox.Extents = Vector3(0.5f, 0.5f, 0.5f) ;

    m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device,context);

    m_currentState = m_search.get();
}

void StraighteningEnemyState::ChangeState(IEnemyState* newState)
{
    //// 新規の状態遷移前に事後更新を行う
    m_currentState->PostUpdate();
    // 新規の状態を現在の状態に設定する
    m_currentState = newState;
    //// 新規の状態遷移後に事前更新を行う
    m_currentState->PreUpdate();
}

void StraighteningEnemyState::PreUpdate()
{
}


//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void StraighteningEnemyState::Update(const float& elapsedTime)
{
    m_position.y -= PlayerState::GRVITY * elapsedTime;
    // 現在の状態を更新する
    m_currentState->Update(elapsedTime);
    m_boundingBox.Center = m_position;
    //押し戻す値
    DirectX::SimpleMath::Vector3 m_pushBack;
    for (auto& wall : m_wall)
    {
        m_pushBack = CheckHitAndResolve(wall->GetBoundingBox());

        // 押し戻しが発生した場合のみポジションを更新
        if (std::abs(m_pushBack.Length()) > 0.00f || std::abs(m_pushBack.Length()) < 0.00f)
        {
            m_position += m_pushBack;
        }
    }
    if (m_position.y < 1.00f)
    {
        m_position.y = 1.00f;
    }
    if (m_hart <= 0)
    {
        isExist = false;
    }

    for (int i = 0; i < 3; i++)
    {
        //触れてないならスキップ
        if (!m_boundingBox.Intersects(m_player->GetBomState(i)->GetBoundingSphere())) continue;

        if (m_player->GetBomState(i)->GetBomPresent() == BomState::MOVEMENT)
        {
            ChangeState(m_exist.get());
            m_player->GetBomState(i)->SetVelocity(Vector3(0, 0, 0));
            m_player->GetBomState(i)->ChangeState(m_player->GetBomState(i)->GetBomStay());
        }
    }
}

void StraighteningEnemyState::PostUpdate()
{

}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void StraighteningEnemyState::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();

    //各描画
    m_currentState->Render(context,view, projection,model);

    //影の位置調整
    m_shadowPosition = m_position;
    m_shadowPosition.y -= 0.45f;
    m_shadowPosition.z += 0.20f;
    // 自機の影を描画する
    m_shadow->Render(context, states, view, projection, m_shadowPosition);
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void StraighteningEnemyState::Finalize()
{
    // 生存をリセットする
    m_exist.reset();
}

//---------------------------------------------------------
// 壁との押し出し判定
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 StraighteningEnemyState::CheckHitAndResolve(
    const DirectX::BoundingBox& wallBox)
{

    using namespace DirectX::SimpleMath;
    // バウンディングボックス同士の衝突判定をとる || 消えているなら、衝突なし
    if (!m_boundingBox.Intersects(wallBox)) { return Vector3::Zero; }
    // AABB用のmin/maxを計算する
    Vector3 aMin = m_boundingBox.Center - m_boundingBox.Extents;
    Vector3 aMax = m_boundingBox.Center + m_boundingBox.Extents;
    Vector3 bMin = wallBox.Center - wallBox.Extents;
    Vector3 bMax = wallBox.Center + wallBox.Extents;

    // 各軸の差分を計算する
    float dx1 = bMax.x - aMin.x;
    float dx2 = bMin.x - aMax.x;
    float dy1 = bMax.y - aMin.y;
    float dy2 = bMin.y - aMax.y;
    float dz1 = bMax.z - aMin.z;
    float dz2 = bMin.z - aMax.z;

    // 各軸について、絶対値の小さい方を軸のめり込み量とする：AABBの重なった部分を特定する
    float dx = std::abs(dx1) < std::abs(dx2) ? dx1 : dx2;
    float dy = std::abs(dy1) < std::abs(dy2) ? dy1 : dy2;
    float dz = std::abs(dz1) < std::abs(dz2) ? dz1 : dz2;

    // 衝突時の押し戻しベクトル
    Vector3 pushBackVec = Vector3::Zero;

    // 衝突閾値を超えた場合のみ押し戻し
    if (std::abs(dx) > 0.0f || std::abs(dy) > 0.0f || std::abs(dz) > 0.0f)
    {
        // まずX軸方向での処理
        if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz))
        {
            pushBackVec.x = dx; // X軸方向で押し戻し
        }
        // 次にZ軸方向での処理
        else if (std::abs(dz) <= std::abs(dy))
        {
            pushBackVec.z = dz; // Z軸方向で押し戻し
        }
    }

    pushBackVec.y = dy;

    if (dy < 0.0f)
    {
        pushBackVec.y = 0.0f;
    }

    // 位置が小刻みに振れる問題を防ぐため、小さな修正量を無視
    if (std::abs(pushBackVec.x) < 0.10f) { pushBackVec.x = 0.0f; }
    ///*if (std::abs(pushBackVec.y) < 0.10f) { pushBackVec.y = 0.0f; }*/
    if (std::abs(pushBackVec.z) < 0.10f) { pushBackVec.z = 0.0f; }

    // めり込みの差の押し返す
    return pushBackVec;
}
