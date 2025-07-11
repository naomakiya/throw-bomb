/*
  @file SelectScene.h
  @brief セレクトシーンクラス
*/
#pragma once
#include"Interface/IScene.h"
#include"Game/UI/Fade.h"

// 前方宣言
class CommonResources;
class SceneManager;
class Sound;

class SelectScene final :  public IScene
{
public:
	enum class Selection { NONE,ONE, TWO,THREE,TITLE };
public:
    // コンストラクト
	SelectScene(SceneManager* sceneManager);
	// デストラクト
	~SelectScene() override;
	// 初期化
	void Initialize(CommonResources* resources) override;
	// 更新
	void Update(float elapsedTime)override;
	// 描画
	void Render() override;
	// 後始末
	void Finalize() override;
	// 次のシーンの獲得
	SceneID GetNextSceneID() const;
private:
	// クラスの生成
	void ClassCreat();
	// リソース読み込み（画像テクスチャなど）
	void LoadResource(ID3D11Device1* device);
	// 画像描画
	void TextureRender();
	// キーボード入力に応じた選択肢の更新
	void UpdateSelection();
private:
	// 共通リソース
	CommonResources* m_commonResources;
	// シーン管理クラス
	SceneManager* m_sceneManager;
	// 音
	std::unique_ptr<Sound> m_sound;

	// 描画関連
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// UI
	std::unique_ptr<UI> m_ui;
	// フェード用の
	std::unique_ptr<Fade> m_fade;

	// テクスチャ（選択肢、背景など）
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_one;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_two;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_three;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_selectBox;

	// 描画・選択関連データ
	// テクスチャの半分の大きさ
	DirectX::SimpleMath::Vector2 m_texCenter;
	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;
	// ビュー行列（固定カメラ用）
	DirectX::SimpleMath::Matrix m_view;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	//セレクト位置
	DirectX::SimpleMath::Vector2 m_slectPos;
	//大きさ
	DirectX::SimpleMath::Vector2 m_scale;
	// 選択時のゆらゆらスケール
	DirectX::SimpleMath::Vector2 m_minScale;
	DirectX::SimpleMath::Vector2 m_maxScale;
	
	// 現在の選んでいる物
	Selection m_currentSelection;

	// 表示や状態関連
	// 透明度
	float m_alpha;
	// 経過時間を計測
	float m_elapsedTime; 
	// 枠の大きさの変更
	float m_selectedScale;
	// シーンチェンジフラグ
	bool m_isChangeScene;
	// 表示状態を切り替えるためのフラグ
	bool m_visible;
	// フェイドのフラグ
	bool m_IsFade;
};