/*
  @file	 GameOver.h
  @brief ゲームオーバークラス
*/
#pragma once
#include"Interface/IScene.h"

// 前方宣言
class CommonResources;
class Sound;
class SceneManager;
class NumberUI;

class GameOver final :	public IScene
{
public:
	enum class Selection { NONE,Select, ReStart, Title };

public:
	// コンスタント
	GameOver(SceneManager* sceneManager);
	// デストラクタ
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
	// リソースマネージャ
	SceneManager* m_sceneManager;
	// 音
	std::unique_ptr<Sound> m_sound;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// 数字
	std::unique_ptr<NumberUI> m_numberUI;
	// 敵の数
	std::unique_ptr<NumberUI> m_sumEnemyNumberUI;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_gameOver;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_select;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_reStart;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_selectBox;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_slash;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_remainingEnemy;
	// 現在選んでいる物
	Selection m_currentSelection;
	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;
	// ビュー行列（固定カメラ用）
	DirectX::SimpleMath::Matrix m_view;
	//セレクトBOX位置
	DirectX::SimpleMath::Vector2 m_slectPos;
	// 枠の大きさの変更
	float m_scale;
	// シーンチェンジフラグ
	bool m_isChangeScene;
	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;

	float m_timer;
};