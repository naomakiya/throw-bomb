/*
  @file  TitleScene.h
  @brief タイトルシーンクラス
*/
#pragma once
#ifndef TITLESCENE_DEFINED
#define TITLESCENE_DEFINED

#include"Interface/IScene.h"
#include"Game/UI/Fade.h"

// 前方宣言
class CommonResources;
class UI;
class TitleOp;

class TitleScene final :  public IScene
{
public:
	// タイトルの選択肢
	enum class Selection { Start, Option, End };
	// 設定の選択肢
	enum class OptionSelection{ BGM,SE,END};
public: 
    //コンストラクト
	TitleScene();
	//デストラクト
	~TitleScene() override;
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
	// クラスを生成する
	void CreateClass();
	//リソースをロードする
	void LoadResource(ID3D11Device1* device);
	//リソース読み込み
	void LoadTexture(ID3D11Device1* device, const std::string& name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& texture);
	// キーボード操作
	void KeyboardOperation();
	//画像を描画
	void TextureRender();
	//	オプションUIの表示
	void RenderOption();
	// オプション操作
	void HandleOptionInput(const DirectX::Keyboard::KeyboardStateTracker* tracker);
	// タイトル操作
	void HandleTitleMenuInput(const DirectX::Keyboard::KeyboardStateTracker* tracker);
	//ボリューム調整
	void AdjustVolume(float delta);
private:
	// 共通リソース
	CommonResources* m_commonResources;
	//UI
	std::unique_ptr<UI> m_ui;
	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// フェード用の
	std::unique_ptr<Fade> m_fade;
	// タイトルOP
	std::unique_ptr<TitleOp> m_TOp;

	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_space;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_end;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_title;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_background;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_optionFreame;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_option;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bgm;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_se;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_box;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_volumeBox;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_close;
	// テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bom;

	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;
	// ビュー行列（固定カメラ用）
	DirectX::SimpleMath::Matrix m_view;
	// 位置
	DirectX::SimpleMath::Vector3 m_position;
	//大きさ
	DirectX::SimpleMath::Vector2 m_scale;
	//透明度
	float m_alpha;
	//経過時間を計測
	float m_elapsedTime; 
	// 選択されたスケール
	float m_selectedScale;
	// 非選択スケール
	float m_unselectedScale;
	//シーンチェンジフラグ
	bool m_isChangeScene;
	// 表示状態を切り替えるためのフラグ
	bool m_visible;
	//フェイドのフラグ
	bool m_IsFade;
	bool m_hasInput;
	// 現在選んでいる物
	Selection m_currentSelection;
	//現在選んでいるオプション
	OptionSelection m_currentOptionSelection;

	bool m_isOptionActive = false;
	int m_optionSelection; // 0: BGM, 1: SE
	float m_bgmVolume;
	float m_seVolume;
	
};
#endif		// TITLESCENE_DEFINED