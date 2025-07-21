/*
  @file StraighteningEnemyDash.h
  @brief ダッシュ敵クラス
*/
#pragma once
#ifndef  STRAIGHTENINGENEMYEXIST_DASH_DEFINED
#define  STRAIGHTENINGENEMYXIST_DASH_DEFINED
#include "Interface/IEnemyState.h"

// 前方宣言
class CommonResources;
class StraighteningEnemyState;
class Wall;
class PlayerState;

class StraighteningEnemyDash :public IEnemyState
{
public:
    // 敵の色変化のの構造体
    struct StateColor
    {
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Vector4 color;
        DirectX::SimpleMath::Vector4 time;
    };
public:
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
    // 現在の向いている方向を取得する
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
public:
     // コンストラクタ
    StraighteningEnemyDash(StraighteningEnemyState* enemyState,
                            const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
     // デストラクタ
     ~StraighteningEnemyDash();
     // 初期化する
     void Initialize(CommonResources* resources);
     // 事前更新する
     void PreUpdate();
     // 更新する
     void Update(const float&  elapsedTime);
     DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& wallBox, const DirectX::BoundingBox& entityBox);
     // 事後更新する
     void PostUpdate();
     // 描画する
     void Render(ID3D11DeviceContext* context,
         DirectX::CommonStates* states,
         const DirectX::SimpleMath::Matrix& view,
         const DirectX::SimpleMath::Matrix& projection,
         const DirectX::Model& model);
     // 後処理を行う
     void Finalize();
     //シェイダーの読み取り
     void ShaderLoad(ID3D11Device* device);
     //バッファーの設定
     void CreateConstanBuffer(ID3D11Device* device);
     //体力の減少
     void HPDown() {};
private:
    //方向ベクトル計算
    void DirectionVectorCalculation();
private:
    // 共通リソース
    CommonResources* m_commonResources;
    //突進する敵
    StraighteningEnemyState* m_enemy;
    //プレイヤ
    PlayerState* m_player;
    //壁
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
	// ワールドマトリックス
	DirectX::SimpleMath::Matrix m_worldMatrix;
    //クォータオン
    DirectX::SimpleMath::Quaternion m_rotation;
	// プレーヤーモデル
	DirectX::Model* m_EnemyModel;
	//モデル
	std::unique_ptr<DirectX::Model> m_model;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotate;

    // Texture用メンバー（テクスチャーとスプライトバッチ）
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_exclamationMarkTexture;
    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    //向く方向
    DirectX::SimpleMath::Vector3 m_direction;
    //ダッシュするまでの時間
    float m_dashTime;
    //生存
    bool m_exist;
    // ヒットフラグ
    bool m_isHit;
 
};
#endif		// PLAYER_DEFINED