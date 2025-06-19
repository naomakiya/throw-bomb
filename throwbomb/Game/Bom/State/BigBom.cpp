/*
  @file Bom.cpp
  @brief プレイヤクラス
*/
#include "pch.h"
#include"Framework/CommonResources.h"
#include"Game/Bom/BigBom.h"
#include"Game/Scene/PlayScene.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/InputManager.h"
#include <cassert>
#include <algorithm>
#include"Framework/Graphics.h"
#include <Libraries/Microsoft/DebugDraw.h>
#include <chrono>
#include <thread>
#include"Game/Wall/Wall.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

//投射投げ
void BigBom::SetExist( DirectX::SimpleMath::Vector3 playerPosition)
{
	// 爆弾の生存を設定
	m_exist = true;

	// 爆弾の生成位置の設定
	m_position = playerPosition;

	// 爆発時間のリセット
	m_explosionTime = 0.0f;
	m_exploded = false;
}


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
BigBom::BigBom(PlayScene* playScene, Graphics* graphics, const std::vector<std::unique_ptr<Wall>>& wall)
	:
	m_position{},
	m_playScene{ playScene },
	m_sphere{},
	m_commonResources{},
	m_mass{0},
	m_exist{false},
	m_gravity{ 0.0f, -9.81f, 0.0f }, // 重力加速度 
	m_graphics(graphics),
	m_BoundingSphere{},
	m_context{},
	m_cnt{},
	m_cntTime{3},
	m_exploded{},
	m_explosionSphere{},
	m_explosionTime{},
	m_currentAngle{0},
	m_wall{wall}

{
	// コンテキストを取得する
	m_context = m_graphics->GetDeviceResources()->GetD3DDeviceContext();
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
BigBom::~BigBom()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void BigBom::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	//auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	//auto states = m_commonResources->GetCommonStates();

	// モデルを読み込む準備
	/*std::unique_ptr<DirectX::DX11::EffectFactory> fx = std::make_unique<DirectX::DX11::EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");*/

	// ジオメトリックプリミティブを生成する
	m_sphere = GeometricPrimitive::CreateSphere(context);
	m_explosionSphere = GeometricPrimitive::CreateSphere(context);

	m_BoundingSphere.Center = m_position;
	m_BoundingSphere.Radius = 0.1f;

	//m_explosionSphere = std::make_unique<DirectX::BoundingSphere>(m_position, 3.0f); // 爆発のスフィアコライダーの初期化
	m_explosionTime = 0.0f;  // 爆発時間のリセット
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void BigBom::Update(const float& elapsedTime)
{
	if (!m_exist) return;

	m_BoundingSphere.Center = m_position;

	if (m_exploded)
	{
		Explode(elapsedTime);
	}
	else
	{
		m_BoundingSphere.Radius = 0.1f;
	}
	
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void BigBom::Render(DirectX::SimpleMath::Matrix view,
	DirectX::SimpleMath::Matrix projection)
{
	/*auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();*/
	// ワールド行列を更新する
	Matrix world = Matrix::CreateTranslation(m_position);

	Matrix worldExp = Matrix::CreateScale(DirectX::SimpleMath::Vector3(3.0f, 3.0f, 3.0f));
	worldExp *= Matrix::CreateTranslation(m_position);
	//モデルを表示する
	if (m_exist)
	{
		m_sphere->Draw(world, view, projection, Colors::Green);
	}
	if (m_exploded)
	{
		m_explosionSphere->Draw(worldExp, view, projection, Colors::Red);

	}

}

// 砲塔境界球を描画する
void BigBom::DrawTurretBoundingSphere()
{
	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// バウンディングスフィアを描画する
	DX::Draw(m_graphics->GetPrimitiveBatch(), m_BoundingSphere, DirectX::Colors::White);

	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();

}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void BigBom::Finalize()
{
}



// 爆発の境界球を描画する
void BigBom::DrawExplosionBoundingSphere()
{
	// プリミティブ描画を開始する
	m_graphics->DrawPrimitiveBegin(m_graphics->GetViewMatrix(), m_graphics->GetProjectionMatrix());

	// 爆発のバウンディングスフィアを描画する
	//DX::Draw(m_graphics->GetPrimitiveBatch(), *m_explosionSphere, DirectX::Colors::Red);

	// プリミティブ描画を終了する
	m_graphics->DrawPrimitiveEnd();
}

//爆発
void BigBom::Explode(const float& elapsedTime)
{
	m_BoundingSphere.Radius = 2.5f;
	m_explosionTime += elapsedTime;
	m_exploded = true;
	if (m_explosionTime >= 1.0f)
	{
		m_exploded = false;
		m_isMove = false;
		m_exist = false;
	}
}

//---------------------------------------------------------
// 衝突判定する
//---------------------------------------------------------
void BigBom::CheckHit(DirectX::BoundingBox boundingBox)
{
	bool isHitWall = false;
	isHitWall = m_BoundingSphere.Intersects(boundingBox);

	if (!isHitWall) { return; }

	// AABBの最小値と最大値を計算
	Vector3 bMin = boundingBox.Center - boundingBox.Extents;
	Vector3 bMax = boundingBox.Center + boundingBox.Extents;

	// 各軸の差分を計算
	float dx1 = bMax.x - m_BoundingSphere.Center.x + m_BoundingSphere.Radius;
	float dx2 = bMin.x - m_BoundingSphere.Center.x - m_BoundingSphere.Radius;
	float dy1 = bMax.y - m_BoundingSphere.Center.y + m_BoundingSphere.Radius;
	float dy2 = bMin.y - m_BoundingSphere.Center.y - m_BoundingSphere.Radius;
	float dz1 = bMax.z - m_BoundingSphere.Center.z + m_BoundingSphere.Radius;
	float dz2 = bMin.z - m_BoundingSphere.Center.z - m_BoundingSphere.Radius;

	// 各軸について、絶対値の小さい方を軸のめり込み量とする
	float dx = std::abs(dx1) < std::abs(dx2) ? dx1 : dx2;
	float dy = std::abs(dy1) < std::abs(dy2) ? dy1 : dy2;
	float dz = std::abs(dz1) < std::abs(dz2) ? dz1 : dz2;

	// 押し戻しベクトル
	Vector3 pushBackVec = Vector3::Zero;

	// めり込みが一番小さい軸を押し戻す
	if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz)) {
		pushBackVec.x += dx;
	}
	else if (std::abs(dz) <= std::abs(dx) && std::abs(dz) <= std::abs(dy)) {
		pushBackVec.z += dz;
	}
	else {
		pushBackVec.y += dy;
	}

	// 押し戻す
	m_position += pushBackVec;
	m_BoundingSphere.Center = m_BoundingSphere.Center + pushBackVec;

	if (pushBackVec.x > 0.0f || pushBackVec.y > 0.0f || pushBackVec.z > 0.0f)
	{
		m_exploded = true;
	}

}