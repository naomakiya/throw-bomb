/*
  @file StraighteningEnemyDash.cpp
  @brief エネミー突進クラス
*/
#include "pch.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemyDash.h"
#include"Game/Enemy/StraighteningEnemyState/StraighteningEnemySearch.h"
#include"Game/Enemy/StraighteningEnemyState.h"
#include"Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include"Game/Wall/Wall.h"
#include"Interface/IEnemyState.h"
#include"Game/Enemy/State/EnemyTracking.h"
#include"Game/Player/PlayerState.h"
#include <Libraries/Microsoft/ReadData.h>


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
StraighteningEnemyDash::StraighteningEnemyDash(StraighteningEnemyState* enemyState,
    const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player)
	:
	m_enemy{ enemyState },
	m_commonResources{},
	m_worldMatrix{},
	m_EnemyModel{},
	m_model{},
	m_position{},
    m_wall{wall},
    m_dashTime{0.0f},
    m_isHit{true},
    m_exist(m_enemy->GetExist()),
    m_scale(m_enemy->GetScale()),
    m_player{player},
    m_boundingBox{},
    m_direction{}
{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
StraighteningEnemyDash::~StraighteningEnemyDash()
{
}

//---------------------------------------------------------
// 初期化
//---------------------------------------------------------
void StraighteningEnemyDash::Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos)
{
   
	assert(resources);
	m_commonResources = resources;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
    auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

    m_position = pos;

    m_boundingBox = m_enemy->GetBoundingBox();

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
//事前更新
//---------------------------------------------------------
void StraighteningEnemyDash::PreUpdate()
{
    m_dashTime = 0.0f;
    m_position = m_enemy->GetPosition();
    m_boundingBox = m_enemy->GetBoundingBox();
}

//---------------------------------------------------------
// 更新
//---------------------------------------------------------
void StraighteningEnemyDash::Update(const float& elapsedTime)
{
    m_dashTime += elapsedTime;

    //方向ベクトルの計算
    DirectionVectorCalculation();

    if (m_dashTime < 2.0f) return; //1秒以下なら飛ばす

    // スタートダッシュ：最初の0.5秒間は急加速
    float speed = (m_dashTime - 1.0f < 0.5f) ? 15.0f : 5.0f;

    // Y座標を変化させないようにする
    m_position.x += m_direction.x * speed * elapsedTime;
    m_position.z += m_direction.z * speed * elapsedTime;

    m_boundingBox.Center = m_position;

    // 突進時間が2秒を超えたら終了して `Search` 状態へ移行
    if (m_dashTime >= 2.5f)m_enemy->ChangeState(m_enemy->GetStraighteningEnemySearch());

    m_enemy->SetPosition(m_position);
    m_enemy->SetBoundingBox(m_boundingBox);
}


//---------------------------------------------------------
// 事後更新
//---------------------------------------------------------
void StraighteningEnemyDash::PostUpdate()
{
   
}

//---------------------------------------------------------
// 描画
//---------------------------------------------------------
void StraighteningEnemyDash::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,const DirectX::SimpleMath::Matrix& projection, const DirectX::Model& model)
{
    using namespace DirectX::SimpleMath;

    auto states = m_commonResources->GetCommonStates();

    // ワールド行列を更新する
    DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateScale(0.006f);
    world *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_rotation)
          * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);

    
    //敵の描画
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

void StraighteningEnemyDash::Finalize()
{
}

//---------------------------------------------------------
// 方向ベクトルの計算
//---------------------------------------------------------
void StraighteningEnemyDash::DirectionVectorCalculation()
{
    // プレイヤーへの方向ベクトルを計算
    m_direction = m_player->GetPosition() - m_position;

    //m_direction.Length(); // ベクトルの長さを取得
    m_direction.Normalize();

    // 進行方向を向くための回転を計算
    m_rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
        atan2(m_direction.x, m_direction.z),  // Yaw (Y軸回転)
        0.0f,  // Pitch
        0.0f   // Roll
    );
}