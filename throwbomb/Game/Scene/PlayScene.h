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
	void ClassCreate();
	// ジェイソンの読み込み
	void loadMapJSON(const char* filename);
	//カメラ処理
	void CameraRotate();
	//坂の当たり判定(プレイヤ）
	void CollisionMeshDeterminationPlayer();
	//坂の当たり判定(ボム）
	void CollisionMeshDeterminationBom();
	//　スカイモデルの描画
	void SkyModelRender();
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
	// 
	SceneManager* m_sceneManager;
	// スカイモデル
	std::unique_ptr<DirectX::Model> m_sky;
	// 壁モデル
	std::unique_ptr<DirectX::Model> m_wallModel;
	// 敵モデル
	std::unique_ptr<DirectX::Model> m_enemyModel;
	// 入力グラフデータ
	std::vector<std::string> m_graphData;
	// マップデータ
	std::vector<DirectX::SimpleMath::Vector3> m_map;
	// マップデータ
	std::vector<int> m_typemap;
	// マップデータ
	std::vector<DirectX::SimpleMath::Vector3> m_enemyMap;
	// マップデータ
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
	// 破壊可能壁モデル
	std::unique_ptr<DirectX::Model> m_Wallcrack;
private:
	//経過時間
	float m_elapsedTime;
	// 体力減った時の次に体力減るまでのクールタイム
	float m_heartCooldownTimer = 0.0f;
	//BGMの音量
	float m_bgmVolume;
	// SEの音量
	float m_seVolume;
	//プレイヤーの場所の保存
	DirectX::SimpleMath::Vector3 m_playerPos;
	// ポリゴンとRayが衝突した座標
	DirectX::SimpleMath::Vector3 m_hitPosition;
	// 衝突したポリゴンの番号
	int m_hitPolygonNo;
	// 衝突したポリゴンの法線の傾き
	DirectX::SimpleMath::Matrix m_rotateNormal;
	// シーンチェンジフラグ
	bool m_isChangeScene;
	// シーンチェンジフラグ(HP）
	bool m_iseEmptyHart;
	//長押しされていたな
	bool isSpaceHeld = false;
	bool on = false;
	bool m_canCountHeart = true;
	bool m_canCreateBomb = false;
	bool m_holdBom = false;
	bool m_se = false;
	bool rotateLeftPressed = false;
	bool rotateRightPressed = false;

};