/*
  @file	 GameOver.h
  @brief ゲームオーバークラス
*/
#pragma once
#include"Interface/IScene.h"

// 前方宣言
class CommonResources;
class ResourceManager;

class GameOver final :	public IScene
{
public:
	// コンスタント
	GameOver(ResourceManager* resourceManager);
	~GameOver() override;
	//　初期化
	void Initialize(CommonResources* resources) override;
	// 更新
	void Update(float elapsedTime)override;
	//　描画
	void Render() override;
	//　処理
	void Finalize() override;
	//　次のシーンの獲得
	SceneID GetNextSceneID() const;
	private:
	// 共通リソース
	CommonResources* m_commonResources;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	//リソースマネージャ
	ResourceManager* m_resourceManager;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clear;
	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;
	// ビュー行列（固定カメラ用）
	DirectX::SimpleMath::Matrix m_view;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	// シーンチェンジフラグ
	bool m_isChangeScene;
	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;
};