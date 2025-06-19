//--------------------------------------------------------------------------------------
// File: ParticleUtil.h
//
// パーティクルユーティリティクラス
//
//-------------------------------------------------------------------------------------

#pragma once

#include "Framework/DeviceResources.h"
#include <SimpleMath.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <Framework/StepTimer.h>

class SmorkUtility
	{
	private:
		//	変数
		float m_initialAngle;

	private:
		//	関数
		//	座標
		DirectX::SimpleMath::Vector3 m_position;
		DirectX::SimpleMath::Vector3 m_centerPosition;
		//	速度
		DirectX::SimpleMath::Vector3 m_velocity;
		//	加速度
		DirectX::SimpleMath::Vector3 m_accele;

		//	スケール
		DirectX::SimpleMath::Vector3 m_nowScale;
		DirectX::SimpleMath::Vector3 m_startScale;
		DirectX::SimpleMath::Vector3 m_endScale;

		//	生存時間
		float m_life;
		float m_startLife;
		float m_rotationAngle;

		//	カラー
		DirectX::SimpleMath::Color m_nowColor;
		DirectX::SimpleMath::Color m_startColor;
		DirectX::SimpleMath::Color m_endColor;

		float m_radius = 1.0f;  // 回転半径

	public:
		//	関数
		//	コンストラクタ（初期設定）
	SmorkUtility(
			float life,
			DirectX::SimpleMath::Vector3 pos,
			DirectX::SimpleMath::Vector3 centerpos,
			DirectX::SimpleMath::Vector3 velocity,
			DirectX::SimpleMath::Vector3 accele,
			DirectX::SimpleMath::Vector3 startScale, DirectX::SimpleMath::Vector3 endScale,
			DirectX::SimpleMath::Color startColor, DirectX::SimpleMath::Color endColor);
		//	デストラクタ
		~SmorkUtility();

		//	更新
		bool Update(float elapsedTime,float radius);

		//	getter
		const DirectX::SimpleMath::Vector3 GetPosition() { return m_position; }
		
		const DirectX::SimpleMath::Vector3 GetVelocity() { return m_velocity; }

		const DirectX::SimpleMath::Vector3 GetAccele() { return m_accele; }
		
		const DirectX::SimpleMath::Vector3 GetNowScale() { return m_nowScale; }
		const DirectX::SimpleMath::Vector3 GetStartScale() { return m_startScale; }
		const DirectX::SimpleMath::Vector3 GetEndScale() { return m_endScale; }

		const float GetAngle() { return m_rotationAngle; }
		const void SetAngle(float rotationAngle) { m_rotationAngle += rotationAngle; }
		const void SetPos(DirectX::SimpleMath::Vector3 pos) { m_position = pos; }
		const float GetLife() { return m_life; }
		const void SetLife(float life) { m_life = life; }
		const float GetStartLife() { return m_startLife; }

		void SetInitialAngle(float angle) { m_initialAngle = angle; }

		const DirectX::SimpleMath::Color GetNowColor() { return m_nowColor; }
		const DirectX::SimpleMath::Color GetStartColor() { return m_startColor; }
		const DirectX::SimpleMath::Color GetEndColor() { return m_endColor; }



};