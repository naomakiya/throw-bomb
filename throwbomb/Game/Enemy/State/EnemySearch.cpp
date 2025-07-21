/*
  @file EnemySearch.cpp
  @brief 探している状態クラス
*/
#include "pch.h"
#include"Game/Enemy/State/EnemySearch.h"
#include"Game/Enemy/EnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Wall/Wall.h"
#include"Game/Enemy/State/EnemyMovement.h"
#include "Game/Player/PlayerState.h"
#include "Game/Enemy/State/EnemyTracking.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
EnemySearch::EnemySearch(EnemyState* enemyState, const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
    : m_enemy(enemyState), 
    m_wall(wall),
    m_currentGoalNo(0),
    m_position(DirectX::SimpleMath::Vector3::Zero),
    m_time(0.0f),
    m_player{player},
    m_commonResources{},
    m_exist{},
    m_scale{0.5f},
    m_modelScale{0.006f},
    m_rotate{},
    m_textureAngle{ 0.0f },
    m_totalTime{0.0f}
{
   
}

//---------------------------------------------------------
// デストラクト
//---------------------------------------------------------
EnemySearch::~EnemySearch()
{
    
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void EnemySearch::Initialize(CommonResources* resources)
{
    m_commonResources = resources;

    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    // SpriteBatch 初期化    
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);

    // テクスチャー読み込み
    DX::ThrowIfFailed(
        DirectX::CreateWICTextureFromFile(
            device,
            L"Resources/Textures/mystery.png", 
            nullptr,
            m_mysteryTexture.ReleaseAndGetAddressOf()
        )
    );
    // バウンディングボックスのサイズを設定（幅2.0、高さ2.0、奥行き2.0の立方体）
    m_searchBoundingBox = DirectX::BoundingOrientedBox(m_position, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.5f), DirectX::SimpleMath::Quaternion::Identity);
}

//---------------------------------------------------------
// 事前更新
//---------------------------------------------------------
void EnemySearch::PreUpdate()
{
    m_position = m_enemy->GetPosition();
    m_time = 0.0f;
    m_textureAngle = 0.0f;
    m_totalTime = 0.0f;
    m_boundingBox = m_enemy->GetBoundingBox();
    m_searchBoundingBox.Center = m_enemy->GetPosition();
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void EnemySearch::Update(const float& elapsedTime)
{
    m_time += elapsedTime;
    m_textureAngle += sin(elapsedTime);
   
    float angleRad = sinf(DirectX::XM_PI * m_time) * (DirectX::XM_PI / 4.0f);
    
    m_textureAngle = angleRad;
    // 1秒で30度回転する
    float rotationSpeed = DirectX::XMConvertToRadians(45.0f) * elapsedTime;

    // Y軸回転のクォータニオンを作成し、現在の回転に適用
    DirectX::SimpleMath::Quaternion rotationDelta = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Up, rotationSpeed);
    m_rotate = m_rotate * rotationDelta;
    m_rotate.Normalize(); // 正規化

    // 敵の前方ベクトルを取得
    DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3::Forward;
    forward = DirectX::SimpleMath::Vector3::Transform(forward, DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotate));

    // 探索用の箱の更新

    // 前方に配置
    m_searchBoundingBox.Center = m_position + forward * -1.5f;
    // 回転を適用
    m_searchBoundingBox.Orientation = m_rotate;

    for (auto& wall : m_wall)
    {
        m_position += CheckHitAndResolve(wall->GetBoundingBox(), m_boundingBox);
    }

    if (m_searchBoundingBox.Intersects(m_player->GetBoundingBox()))
    {
        m_enemy->ChangeState(m_enemy->GetEnemyTracking());
    }

    if (m_time > 5.0f)
    {
        m_enemy->ChangeState(m_enemy->GetEnemyMovement());
    }

    this->PostUpdate();
}

//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void EnemySearch::PostUpdate()
{
    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void EnemySearch::Render(ID3D11DeviceContext* context, DirectX::CommonStates* states, 
    const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection,
    const DirectX::Model& model)
{
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
        m_mysteryTexture.Get(),       // テクスチャ
        drawPos,                      // スクリーン描画位置（中心）
        nullptr,                      // RECT
        DirectX::Colors::White,       // 色
        m_textureAngle,                         // 回転角
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
void EnemySearch::Finalize()
{
}

//---------------------------------------------------------
// 壁との押し出し判定
//---------------------------------------------------------
DirectX::SimpleMath::Vector3 EnemySearch::CheckHitAndResolve(
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