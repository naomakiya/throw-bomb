/*
  @file  Goal.h
  @brief ゴールクラス
*/

#pragma once
#ifndef GAOL_DEFINED
#define GAOL_DEFINED
#include "GeometricPrimitive.h"

// 前方宣言
class CommonResources;

class Goal
{
public:
	// ポイントライトの構造体
	struct PointLight
	{
		DirectX::SimpleMath::Vector4 position;	// ワールド座標
		DirectX::SimpleMath::Vector4 color;		// ライトの色
		DirectX::SimpleMath::Vector4 time;		// 時間パラメータ
	};

public:
	// スケールを取得する
	float GetScale() const { return m_scale; }
	// 位置を取得する
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// バウンディングボックスを取得する
	DirectX::BoundingSphere GetBoundingSphere() const { return m_boundingSphere; }

public:
	// コンストラクタ
	Goal(DirectX::SimpleMath::Vector3 pos);
	//デストラクタ
	~Goal();
	//初期化する
	void Initialize(CommonResources* resources);
	//描画する
	void Render(ID3D11DeviceContext* context,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection) ;
	//後処理を行う
	void Finalize();

private:
	//モデル読み取り
	void LoadModel(ID3D11Device* device);
	//シェイダーの読み取り
	void LoadShader(ID3D11Device* device);
	//バッファーの設定
	void CreateConstanBuffer(ID3D11Device* device);

private:
	// 共通リソース
	CommonResources* m_commonResources;
	// ゴールのモデル
	std::unique_ptr<DirectX::Model> m_model;
	//ピクセルシェイダー
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
	//インプットレイヤー
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	//定数用バッファー
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	// バウンディングボックス
	DirectX::BoundingSphere m_boundingSphere;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// スケール
	float m_scale;
	//時間の作成
	float m_time;
};
#endif		// PLAYER_DEFINED