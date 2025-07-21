/*
  @file  EnemyExist.h
  @brief 敵の生存状態クラス
*/
#pragma once
#ifndef  ENEMYEXIST_DEFINED
#define  ENEMYEXIST_DEFINED
#include "Interface/IEnemyState.h"

// 前方宣言
class CommonResources;
class EnemyState;
class Wall;

class EnemyExist :public IEnemyState
{
public:
    // 位置を取得する
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // 位置を設定する
    void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
    // 目的地の位置を設定(ここでは使用なし）
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //敵の生存取得
    bool GetExist() const { return m_isExist; }
    //敵の生存設定
    void SetExist(const bool isexist) { m_isExist = isexist; }
    // バウンディングボックスを取得する
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // バウンディングボックスを取得する
    void SetBoundingBox(const DirectX::BoundingBox box) { m_boundingBox = box; }
    // 現在の向いている方向を取得する
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }

public:
     // コンストラクタ
     EnemyExist(EnemyState* enemyState);
     // デストラクタ
     ~EnemyExist();
     // 初期化する
     void Initialize(CommonResources* resources);
     // 事前更新する
     void PreUpdate();
     // 更新する
     void Update(const float&  elapsedTime);
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
     //　HPを下げる (このクラスでは使用しない)
     void HPDown() {};

private:
    // 共通リソース
    CommonResources* m_commonResources;
    //索敵の敵
    EnemyState* m_enemy;
    // バウンディングボックス
    DirectX::BoundingBox m_boundingBox;
    // 位置
    DirectX::SimpleMath::Vector3 m_position;
    // 敵の回転
    DirectX::SimpleMath::Quaternion m_rotate;
    // 時間
    float m_time;
    //生存
    bool m_isExist;
};
#endif		// ENEMYEXIST_DEFINED