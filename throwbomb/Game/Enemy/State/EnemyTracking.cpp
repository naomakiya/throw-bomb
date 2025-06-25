/*
  @file EnemyTracking.cpp
  @brief 敵の追いかける状態クラス
*/
#include "pch.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Enemy/EnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Wall/Wall.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include "Game/Enemy/State/EnemySearch.h"
#include "Game/Player/PlayerState.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemyTracking::EnemyTracking(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
    : m_enemy(enemyState), 
    m_wall(wall),
    m_currentGoalNo(0),
    m_position(DirectX::SimpleMath::Vector3::Zero),
    m_time(0.0f),
    m_player{player},
    m_commonResources{},
    m_exist{},
    m_scale{0.5f},
    m_modelScale{0.006f}
{
   
}

//---------------------------------------------------------
// デストラクト
//---------------------------------------------------------
EnemyTracking::~EnemyTracking()
{
    
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void EnemyTracking::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
    m_commonResources = resources;
    m_position = pos;

    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    m_boundingBox.Center = m_position;
    m_boundingBox.Center.y += 0.5f;
    m_boundingBox.Extents = DirectX::SimpleMath::Vector3(m_scale);

    // SpriteBatch 初期化
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

    // テクスチャー読み込み
    DX::ThrowIfFailed(
        DirectX::CreateWICTextureFromFile(
            device,
            L"Resources/Textures/exclamationMark.png",
            nullptr,
            m_exclamationMarkTexture.ReleaseAndGetAddressOf()
        )
    );
   
}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void EnemyTracking::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_time = 0.0f;
    
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void EnemyTracking::Update(const float& elapsedTime)
{
    m_time += elapsedTime;

    // プレイヤーの位置に向かって移動する処理
    DirectX::SimpleMath::Vector3 targetPos = m_player->GetPosition();
    // 高さ補正
    targetPos.y += 0.5f;  

    DirectX::SimpleMath::Vector3 direction = targetPos - m_position;

    // ベクトルの長さを取得
    float distance = direction.Length(); 
    // 方向を正規化する
    direction.Normalize();

    // 進行方向に基づく回転を計算
    if (direction.LengthSquared() > 0.0f)
    {
        // Y軸を中心に回転する場合
        m_rotate = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
            atan2(direction.x, direction.z),  // Yaw (Y軸周りの回転)
            0.0f,                               // Pitch
            0.0f                                // Roll
        );
    }

    if (distance > 0.1f) // 一定の距離以上であれば移動
    {
        float speed = 2.0f;     // 敵の移動速度 (調整可能)
        m_position += direction * speed * elapsedTime; // プレイヤーの方向に移動
    }

    for (auto& wall : m_wall)
    {
        m_position += CheckHitAndResolve(wall->GetBoundingBox(), m_boundingBox);
    }

    m_boundingBox.Center = m_position;
    m_enemy->SetBoundingBox(m_boundingBox);

    if (m_time > 5.0f)
    {
        m_enemy->ChangeState(m_enemy->GetEnemySearch());
    }

    this->PostUpdate();
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void EnemyTracking::PostUpdate()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void EnemyTracking::Render(ID3D11DeviceContext* context, const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
    auto states = m_commonResources->GetCommonStates();
    // ワールド行列を更新する
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(m_modelScale);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotate);
    world *= DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
    // モデルを表示する
    model.Draw(context, *states, world, view, projection);

    // === 敵の「頭上」1mの座標をまず計算 ===
    DirectX::SimpleMath::Vector3 headPosition = m_position + DirectX::SimpleMath::Vector3(0, 1.0f, 0);

    // === 3D → 2Dスクリーン座標変換 ===
    DirectX::XMVECTOR screenPosVec = DirectX::XMVector3Project(
        DirectX::XMLoadFloat3(&headPosition),
        0.0f, 0.0f,
        static_cast<float>(m_commonResources->GetDeviceResources()->GetOutputSize().right),
        static_cast<float>(m_commonResources->GetDeviceResources()->GetOutputSize().bottom),
        0.0f, 1.0f,
        projection,
        view,
        DirectX::SimpleMath::Matrix::Identity
    );

    DirectX::SimpleMath::Vector3 screenPos2D;
    DirectX::XMStoreFloat3(&screenPos2D, screenPosVec);

    // === スプライト描画 ===
    m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());

    float textureWidth = 726.0f;
    float textureHeight = 468.0f;
    float scale = 0.1f; // 小さめに調整

    DirectX::SimpleMath::Vector2 origin(textureWidth * 0.5f, textureHeight * 0.5f);
    DirectX::SimpleMath::Vector2 drawPos(screenPos2D.x, screenPos2D.y);

    // テクスチャ描画：drawPosがスクリーン座標の中心位置になる
    m_spriteBatch->Draw(
        m_exclamationMarkTexture.Get(),       // テクスチャ
        drawPos,                      // スクリーン描画位置（中心）
        nullptr,                      // RECT
        DirectX::Colors::White,       // 色
        0.0f,                         // 回転角
        origin,                       // テクスチャの原点（中央）
        scale,                        // スケーリング
        DirectX::SpriteEffects_None,  // エフェクト
        0.0f                          // レイヤー深度
    );

    m_spriteBatch->End();
}

//---------------------------------------------------------
// 処理
//---------------------------------------------------------
void EnemyTracking::Finalize()
{
}

//---------------------------------------------------------
// 壁との押し出し判定
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 EnemyTracking::CheckHitAndResolve(
    const DirectX::BoundingBox& wallBox, const DirectX::BoundingBox& entityBox)
{
    using namespace DirectX::SimpleMath;

    // 交差していなければ押し出しなし
    if (!wallBox.Intersects(entityBox))   return Vector3::Zero;

    // 押し出しベクトルを計算する
    Vector3 pushOut = Vector3::Zero;

    Vector3 wallCenter = wallBox.Center;
    Vector3 entityCenter = entityBox.Center;
    Vector3 wallExtents = wallBox.Extents;
    Vector3 entityExtents = entityBox.Extents;

    // 各軸ごとの押し出し量を計算
    float deltaX = std::max(0.0f, (wallExtents.x + entityExtents.x) - abs(entityCenter.x - wallCenter.x));
    float deltaZ = std::max(0.0f, (wallExtents.z + entityExtents.z) - abs(entityCenter.z - wallCenter.z));

    // X方向とZ方向のどちらの押し出しが小さいかで決定
    if (deltaX < deltaZ)
    {
        pushOut.x = (entityCenter.x > wallCenter.x) ? deltaX : -deltaX;
    }
    else
    {
        pushOut.z = (entityCenter.z > wallCenter.z) ? deltaZ : -deltaZ;
    }

    return pushOut;
}