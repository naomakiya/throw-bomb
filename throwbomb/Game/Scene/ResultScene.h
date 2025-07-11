/*
  @file ResultScene.h
  @brief リザルトシーンクラス
*/
#pragma once
#include"Interface/IScene.h"

// 前方宣言
class CommonResources;
class Sound;
class SceneManager;
class NumberUI;

class ResultScene final :
	public IScene
{
public:
	enum class Selection { Select,ReStart, Title };
public:
	//コンストラクト
	ResultScene(SceneManager* sceneManager);
	//デストラクト
	~ResultScene() override;
	//イニシャライズ
	void Initialize(CommonResources* resources) override;
	//更新
	void Update(float elapsedTime)override;
	//描画
	void Render() override;
	//後始末
	void Finalize() override;
	//次のシーンの獲得
	SceneID GetNextSceneID() const;
private:
	//リソースをロードする
	void LoadResource(ID3D11Device1* device);
	//画像を描画
	void TextureRender();
private:
	// 共通リソース
	CommonResources* m_commonResources;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// 音
	std::unique_ptr<Sound> m_sound;
	// シーンマネージャ
	SceneManager* m_sceneManager;
	// 数字UI
	std::unique_ptr<NumberUI> m_numberUI;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clear;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_backGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_select;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_reStart;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_scoreTextur;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_selectBox;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_aScore;
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
	// 大きさ
	float m_scale;
	// 現在選んでいる物
	Selection m_currentSelection;
	//セレクトBOXの位置
	DirectX::SimpleMath::Vector2 m_slectPos;
	//スコア
	float m_score;
	//シーンチェンジPlayScene
	bool m_replay;
	//シーンチェンジSelectScene
	bool m_isSelect;
	//経過時間を計測
	float m_elapsedTime;
	float m_selectedScale;
	float m_unselectedScale;
	
};