/*
  @file PlayScene.h
  @brief プレイシーンクラス
*/
#pragma once
#include"Interface/IScene.h"

#include"nlohmann/json.hpp"
using json = nlohmann::json;

// 前方宣言
class CommonResources;
class PlayerState;
class Floor;
class Camera;
class Wall;
class UI;
class Goal;
class ResourceManager;
class Star;
class Vase;
class EnemyManager;
class Fade;
class SceneManager;

namespace mylib
{
	class CollisionMesh;
}

class PlayScene final :	public IScene
{
public:
	// コンストラクト
	PlayScene(SceneManager* sceneManager);
	// デストラクタ
	~PlayScene() override;
	// 初期化
	void Initialize(CommonResources* resources) override;
	// 更新
	void Update(float elapsedTime)override;
	// 描画
	void Render() override;
	// 処理
	void Finalize() override;
	// 次のシーンの獲得
	SceneID GetNextSceneID() const;

private:
	// クラスの生成
	void CreateClass();
	// ジェイソンの読み込み
	void loadMapJSON(const char* filename);
	//カメラ処理
	void CameraRotate();
	//坂の当たり判定(プレイヤ）
	void CheckPlayerCollisionMesh();
	//坂の当たり判定(ボム）
	void CheckBomCollisionMesh();
	//　スカイモデルの描画
	void RenderSkyModel();
	// リザルトチェンジ用の変数の設定
	void RequestResultScene();

private:
	// 共通リソース
	CommonResources* m_commonResources;
	// 射影行列
	DirectX::SimpleMath::Matrix m_projection;
	// ビュー行列（固定カメラ用）
	DirectX::SimpleMath::Matrix m_view;
	// TPSカメラ
	std::unique_ptr<Camera> m_camera;
	// プレイヤー
	std::unique_ptr<PlayerState> m_player;
	//敵の管理
	std::unique_ptr<EnemyManager> m_enemyManager;
	// 床
	std::vector<std::shared_ptr<Floor>> m_floors;
	// 壁
	std::vector<std::unique_ptr<Wall>> m_walls;
	// UI
	std::unique_ptr<UI> m_ui;
	// ゴール
	std::unique_ptr<Goal> m_goal;
	// シーンマネージャ
	SceneManager* m_sceneManager;
	// スカイモデル
	std::unique_ptr<DirectX::Model> m_sky;
	// マップデータ
	std::vector<DirectX::SimpleMath::Vector3> m_map;
	// 壁の種類マップデータ
	std::vector<int> m_typemap;
	// 敵の位置のマップデータ
	std::vector<DirectX::SimpleMath::Vector3> m_enemyMap;
	// 突進敵の位置のマップデータ
	std::vector<DirectX::SimpleMath::Vector3> m_straighteningEnemyMap;
	// 坂の位置マップデータ
	std::vector<DirectX::SimpleMath::Vector3> m_collisionMeshPosMap;
	// 種類の位置マップデータ
	std::vector<int> m_collisionMeshTypeMap;
	// 回転の位置マップデータ
	std::vector<DirectX::SimpleMath::Quaternion> m_collisionMeshRotationMap;
	// 敵の経路情報の保存
	std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
	// 壺のマップデータ
	std::vector<DirectX::SimpleMath::Vector3> m_vasePosMap;
	// コリジョンメッシュクラス
	std::vector<std::unique_ptr<mylib::CollisionMesh>> m_collisionMesh;
	// 壺
	std::vector<std::unique_ptr<Vase>> m_vase;
	// フェイド
	std::unique_ptr<Fade> m_fade;

public:
	// 強く押し返す
	static constexpr float PUSHBACKSTRENGTH = 0.5f;
	//　弱く押し返す
	static constexpr float SMALLPUSHBACKSTRENGTH = 0.01f;

private:
	// 衝突したポリゴンの法線の傾き
	DirectX::SimpleMath::Matrix m_rotateNormal;
	// プレイヤーの場所の保存
	DirectX::SimpleMath::Vector3 m_playerPos;
	// ポリゴンとRayが衝突した座標
	DirectX::SimpleMath::Vector3 m_hitPosition;
	// 経過時間
	float m_elapsedTime;
	// 体力減った時の次に体力減るまでのクールタイム
	float m_downheartCoolTimer;
	// BGMの音量
	float m_bgmVolume;
	// SEの音量
	float m_seVolume;
	// 衝突したポリゴンの番号
	int m_hitPolygonNo;
	// シーンチェンジフラグ
	bool m_isChangeScene;
	// シーンチェンジフラグ(HP）
	bool m_isEmptyHart;
	// ダメージを受けたか
	bool m_isDamageHeart;
	// 左回転中か
	bool m_isrotateLeftPressed;
	// 右回転中か
	bool m_isrotateRightPressed;

};