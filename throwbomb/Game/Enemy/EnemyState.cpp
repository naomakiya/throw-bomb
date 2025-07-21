/*
  @file  EnemyState.cpp
  @brief 敵の状態クラス
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Enemy/EnemyState.h"
#include "Game/Enemy/State/EnmyExist.h"
#include "Game/Enemy/State/EnemyMovement.h"
#include "Game/Enemy/State/EnemyTracking.h"
#include "Game//Enemy/State/EnmyStop.h"
#include "Game/Enemy/State/EnemyPointMovement.h"
#include "Game/Enemy/State//EnemySearch.h"
#include "Game/Player/PlayerState.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomStay.h"
#include "Game/Shadow/Shadow.h"

void EnemyState::HPDown()
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

    //体力がないなら生存をfalseにする
    if (m_hart <= 0) isExist = false;
}

void EnemyState::SetPointPosition(DirectX::SimpleMath::Vector3& position)
{ 
    m_pointPosition = position;
    ChangeState(m_pointMovement.get());
    
}

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemyState::EnemyState( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath, const DirectX::SimpleMath::Vector3& position)
    :
    m_position{ position },
    m_enemyModel{},
    m_commonResources{},
    m_angle{ 0 },
    m_scale{ 0.5f },
    m_currentState{},
    m_player{player},
    m_totalSeconds{},
    m_wall{wall},
    isExist{true},
    m_hart{5},
    m_patrolPath{ patrolPath },
    m_boudingSphere{},
    m_collisionDebugRenderer{},
    m_shadow{},
    m_stop{},
    m_pointMovement{}
{
   
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
EnemyState::~EnemyState()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void EnemyState::Initialize(CommonResources* resources)
{
    assert(resources);
    m_commonResources = resources;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
    //中心の移動
    m_position.y += 0.5f;

    m_boudingSphere.Center = m_position;
    m_boudingSphere.Radius = 0.25f;

    // 影を作成する
    m_shadow = std::make_unique<Shadow>();
    m_shadow->Initialize(device, context);

    m_exist = std::make_unique<EnemyExist>(this);
    m_exist->Initialize(m_commonResources);

    m_movement = std::make_unique<EnemyMovement>(this, m_wall, m_patrolPath);
    m_movement->Initialize(m_commonResources);

    m_pointMovement = std::make_unique<EnemyPointMovement>(this, m_wall);
    m_pointMovement->Initialize(m_commonResources);

    m_tracking = std::make_unique<EnemyTracking>(this, m_wall,m_player);
    m_tracking->Initialize(m_commonResources);

    m_stop = std::make_unique<EnemyStop>(this, m_wall);
    m_stop->Initialize(m_commonResources);

    m_search = std::make_unique<EnemySearch>(this, m_wall, m_player);
    m_search->Initialize(m_commonResources);
    
    m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device,context);

    m_currentState = m_exist.get();
}

//---------------------------------------------------------
// 状態を変える
//---------------------------------------------------------
void EnemyState::ChangeState(IEnemyState* newState)
{
    //// 新規の状態遷移前に事後更新を行う
    m_currentState->PostUpdate();
    // 新規の状態を現在の状態に設定する
    m_currentState = newState;
    //// 新規の状態遷移後に事前更新を行う
    m_currentState->PreUpdate();
}

//---------------------------------------------------------
// 事前更新する
//---------------------------------------------------------
void EnemyState::PreUpdate()
{
    
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void EnemyState::Update(const float& elapsedTime)
{
    // 現在の状態を更新する
    m_currentState->Update(elapsedTime);
   //爆弾の数回す
    for (int i = 0; i < 3; i++)
    {   
        //触れてないならスキップ
        if (!m_boundingBox.Intersects(m_player->GetBomState(i)->GetBoundingSphere())) continue;

        //爆弾が移動しているなら敵を止める
        if (m_player->GetBomState(i)->GetBomPresent() == BomState::MOVEMENT)
        {
            ChangeState(m_stop.get());
            m_player->GetBomState(i)->SetVelocity(DirectX::SimpleMath::Vector3(0, 0, 0));
            m_player->GetBomState(i)->ChangeState(m_player->GetBomState(i)->GetBomStay());
        }
    }
    
}

//---------------------------------------------------------
// 事後更新する
//---------------------------------------------------------
void EnemyState::PostUpdate()
{

}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void EnemyState::Render(ID3D11DeviceContext* context,
    DirectX::CommonStates* states,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
    // 現在の状態を描画する
    m_currentState->Render(context,states,view, projection,model);
    // 自機の影を描画する
    m_shadow->Render(context, states, view, projection, m_position);

#ifdef _DEBUG
    //デバックのカラー情報の作成
    const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
        DirectX::Colors::Red.f[1],
        DirectX::Colors::Red.f[2],
        DirectX::Colors::Red.f[3]);

    // デバッグ描画
    m_collisionDebugRenderer->RenderBoundingBox(m_boundingBox, view, projection, color);
#endif
}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void EnemyState::Finalize()
{
   
}


