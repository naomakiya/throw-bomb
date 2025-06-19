//--------------------------------------------------------------------------------------
// File: SmorkUtil.cpp
//
// パーティクルユーティリティクラス
// positionや速度などパーティクル１つに必要な要素を格納
// 更新時に生存時間や座標、大きさなどを再計算し、
// 寿命が尽きたらfalseを返して削除を促す
//
//-------------------------------------------------------------------------------------

#include "pch.h"
#include "SmorkUtility.h"

#include "Framework/StepTimer.h"
#include "Framework/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>

using namespace DirectX;

const static float ENDLESS = -100.0f;

/// <summary>
/// コンストラクタ
/// 引数は初期設定用の数値
/// </summary>
/// <param name="life">      生存時間</param>
/// <param name="pos">       座標</param>
/// <param name="velocity">  速度</param>
/// <param name="accele">    加速度</param>
/// <param name="startScale">初期サイズ</param>
/// <param name="endScale">  最終サイズ</param>
/// <param name="startColor">初期色</param>
/// <param name="endColor">  最終色</param>
SmorkUtility::SmorkUtility(
	float life,
	DirectX::SimpleMath::Vector3 pos,
	DirectX::SimpleMath::Vector3 centerpos,
	DirectX::SimpleMath::Vector3 velocity,
	DirectX::SimpleMath::Vector3 accele,
	DirectX::SimpleMath::Vector3 startScale, DirectX::SimpleMath::Vector3 endScale,
	DirectX::SimpleMath::Color startColor, DirectX::SimpleMath::Color endColor)
{
	m_rotationAngle = 0;
	//	与えられた生存時間（s）を取得
	m_startLife =
		m_life = life;

	//現在の角度を取得
	m_initialAngle = 0.0f;
	//	初期位置を取得
	m_position = pos;
	m_centerPosition = centerpos;
	//	移動ベクトルを取得
	m_velocity = velocity;
	//	加速度ベクトルを取得
	m_accele = accele;

	//	初めのサイズを取得
	m_startScale = startScale;
	m_nowScale = startScale;
	//	終了のサイズを取得
	m_endScale = endScale;

	//	初めの色を取得
	m_startColor = startColor;
	m_nowColor = startColor;
	//	終了の色を取得
	m_endColor = endColor;
}
/// <summary>
/// デストラクタ
/// </summary>
SmorkUtility::~SmorkUtility()
{
}
/// <summary>
/// 更新関数
/// </summary>
/// <param name="timer">Game等からStepTimerを受け取る</param>
/// <returns>生存時間(life)がある間はtrue</returns>
bool SmorkUtility::Update(float elapsedTime, float radius)
{
	// 角度を時間経過に応じて増加させる
	float rotationSpeed = 0.25f;  // 回転速度（ラジアン/秒）

	m_rotationAngle += rotationSpeed * elapsedTime;  // 角度更新

	//	スケール。現在の生存時間から、大きさをLerpで算出する
	m_nowScale = SimpleMath::Vector3::Lerp(m_startScale, m_endScale, 1.0f - m_life / m_startLife);
	//	色。現在の生存時間から、色をLerpで算出する
	m_nowColor = SimpleMath::Color::Lerp(m_startColor, m_endColor, 1.0f - m_life / m_startLife);

	float angle = m_initialAngle + m_rotationAngle;

	m_position.x = m_centerPosition.x + radius * cos(angle);
	m_position.z = m_centerPosition.z + radius * sin(angle);
	
	//	ライフを減らしていく
	m_life -= elapsedTime;

	//	ライフが0未満なら自身を消してもらう
	if (m_life < 0.0f)
	{
		return false;
	}

	return true;
}
