/*
	@file	PlayScene.cpp
	@brief	プレイシーンクラス
*/

#include "pch.h"
#include "PlayScene.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "SceneManager.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game//Factory/WallFactory.h"
#include "Game/Factory/CollisionMeshFactory.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/BomState.h"
#include "Game/Goal/Goal.h"
#include "Game/Floor/Floor.h"
#include "Game/Vase/Vase.h"
#include "Game/Camera/Camera.h"
#include "Game/Collision/Collision.h"
#include "Game/UI/UI.h"
#include "Game/UI/Fade.h"
#include "Game/UI/Number/Number.h"
#include "Game/Sound/Sound.h"

//nlohmann/json.hppライブラリ
#include"nlohmann/json.hpp"
#include <Framework/LoadJson.h>
using json = nlohmann::json;

//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayScene::PlayScene(SceneManager* sceneManager)
	:
	m_commonResources{},
	m_projection{},
	m_isChangeScene{false},
	m_player{},
	m_floors{},
	m_camera{},
	m_view{},
	m_walls{},
	m_ui{},
	m_goal{},
	m_sky{},
	m_sceneManager{ sceneManager },
	m_map{},
	m_fade{},
	m_vase{},
	m_hitPolygonNo{},
	m_enemyMap{},
	m_typemap{},
	m_isEmptyHart{false},
	m_straighteningEnemyMap{},
	m_enemyManager{},
	m_elapsedTime{},
	m_seVolume{},
	m_bgmVolume{},
	m_collisionMeshPosMap{},
	m_collisionMeshRotationMap{},
	m_collisionMeshTypeMap{},
	m_isDamageHeart{false},
	m_downheartCoolTimer{0.0f},
	m_isrotateLeftPressed{false},
	m_isrotateRightPressed{false}
{
	
}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
PlayScene::~PlayScene()
{
	Finalize();
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void PlayScene::Initialize(CommonResources* resources)
{
	using namespace DirectX;

	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// デバッグカメラを作成する
	RECT rect{ m_commonResources->GetDeviceResources()->GetOutputSize() };

	// 射影行列を作成する
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.15f, 1000.0f
	);


	//　ステージ番号の取得
	int number = m_sceneManager->GetSelectNumber();

	// int を std::string に変換
	std::string numberString = std::to_string(number);

	// マップ情報をJSONファイルから読み込んで作成
	loadMapJSON(ResourceManager::getStagePath(numberString).c_str());

	// クラス生成
	CreateClass();


	//読み込み準備
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//スカイドーム
	m_sky = Model::CreateFromCMO(device, ResourceManager::getModelPath("SkyDome").c_str(), *fx);


	// 音量の読み込み
	LoadJson json("Resources/Json/Music.json");
	// BGMの音量の設定
	m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
	//　SEの音量の設定
	m_seVolume = json.GetJson()["SE"].value("Volume", 0.0f);
	// サウンドのインスタンスの獲得
	Sound::GetInstance().Initialize();
	// BGMの再生
	Sound::GetInstance().PlayBGM(ResourceManager::getBGMPath("PlayBGM").c_str(), true);
	// 音量の設定
	Sound::GetInstance().SetVolume(m_bgmVolume);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	//時間経過
	m_elapsedTime = elapsedTime;
	
	// プレイヤーを更新する
	m_player->Update(elapsedTime);

	//Uiの更新
	m_ui->Update();

	//カメラの回転キー
	CameraRotate();

	//カメラ更新
	m_camera->UpdateCameraRotation(elapsedTime);

	// カメラの位置をプレイヤーの位置に連動させる
	m_camera->Update(m_player->GetPosition());

	// ビューの情報の取得
	m_view = m_camera->GetViewMatrix();

	//敵の更新
	m_enemyManager->Update(elapsedTime);

	//Rayを使用してコリジョンメッシュと衝突判定をとる(プレイヤー）
	CheckPlayerCollisionMesh();

	//Rayを使用してコリジョンメッシュと衝突判定をとる(ボム）
	for (int i = 0; i < PlayerState::BOMMAX; i++){
		//	ボムが有るのなら判定をする
		if (m_player->GetBomState(i)->GetExist()){	
			CheckBomCollisionMesh();
		}
	}

	//壺とプレイヤーの当たり判定
	for (auto& vase : m_vase){
		// 壺が有るか？
		if (vase->GetExist()){
			// 衝突判定の結果をプレイヤに設定する
			m_player->SetPosition(m_player->GetPosition() - 
				mylib::Collision::CheckHit(vase->GetBoundingBox(), m_player->GetBoundingBox()));
			//ボムの判定処理
			for (int i = 0; i < PlayerState::BOMMAX; i++){
				// 爆発状態出ないなら飛ばす
				if (m_player->GetBomState(i)->GetBomPresent() == BomState::BomtPresent::EXPLOSION) continue;
				// 爆発と壺が当たっているか
				if (mylib::Collision::BoundingCheckCollision(vase->GetBoundingBox(),
					m_player->GetBomState(i)->GetBoundingSphere())){
					// 存在を消す
					vase->SetExist(false);
				}
			}
		}
		else{
			// 破壊後の処理
			vase->BreacVase();
			// 一定の範囲以下なら敵を壺の位置へ誘導する
			if (vase->GetBoundingSphere().Radius < 15.0){
				// 誘導する位置を設定する
				m_enemyManager->SetVacePos(vase->GetPosition());
			}
		}
	}

	// プレイヤーが敵にヒットしたかどうかを確認
	if (!m_player->GetExist() && !m_isDamageHeart){
		// ハートの減少
		m_ui->CountHeart();
		// 一度だけ呼び出すためにフラグをオフにする
		m_isDamageHeart = true;
		// クールダウンタイマーをリセット
		m_downheartCoolTimer = 0.0f;
	}

	// クールダウンタイマーを更新
	if (m_isDamageHeart){
		//時間を計測する
		m_downheartCoolTimer += elapsedTime;
		// 3秒後に再び呼び出せるようにフラグをオンにする
		if (m_downheartCoolTimer >= 3.0f){
			m_isDamageHeart = false;
		}
	}

	// ゴールに触れた場合はシーンを変更
	if (mylib::Collision::BoundingCheckCollision(m_player->GetBoundingBox(), m_goal->GetBoundingSphere())){
		// シーンの変更を許可する
		m_isChangeScene = true;
		// フェイド用の時間を設定
		m_fade->SetTime(0.0f);
	}

	// ハートが全て空になったらリザルトシーンに遷移する
	if (m_ui->AreHeartsEmpty() && !m_isEmptyHart){
		RequestResultScene();
	}
	// 時間制限になったらシーンの変更
	if (m_ui->GetNumber()->GetTime() <= 0.0f && !m_isEmptyHart){
		RequestResultScene();
	}

	//敵が全滅したらクリア
	if (m_enemyManager->GetEmpty() && !m_isChangeScene){
		// シーンの変更を許可する
		m_isChangeScene = true;
		// フェイド用の時間を設定
		m_fade->SetTime(0.0f);
	}
}


//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayScene::Render()
{
	using namespace DirectX;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states =  m_commonResources->GetCommonStates();

	// 複数の床を描画する
	for (auto& floor : m_floors){
		floor->Render(m_view, m_projection);
	}

	// 壁の描画
	for (const auto& wall : m_walls){	
		// 表示されているなら描画
		if (!wall->IsVisible())wall->Render(context, m_view, m_projection);
	}

	// メッシュを描画する
	for (auto& collisionMesh : m_collisionMesh){
		collisionMesh->Draw(context, states, m_view, m_projection);
	}

	// 壺の描画
	for (auto& vase : m_vase){
		// 壺がいる描画
		if (vase->GetExist()) vase->Render(context, m_view, m_projection);
	}

	//敵の描画
	m_enemyManager->Render(context, m_view, m_projection);

	// プレイヤーを描画する
	m_player->Render(m_view, m_projection);

	//スカイモデルの描画
	RenderSkyModel();

	//ゴールの描画
	m_goal->Render(context, m_view, m_projection);

	//UIの描画
	m_ui->Render(m_elapsedTime);

	//フェイドの表示
	if (m_fade->GetTime() < 3.25f){
		// 時間経過
		m_fade->Timer(m_elapsedTime);
		//　フェイドの描画
		m_fade->Render(m_ui.get());
	}


}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void PlayScene::Finalize()
{
	// 全てのリソースの解放を行う
	m_floors.clear();
	m_walls.clear();
	m_player.reset();
	m_ui.reset();
	m_goal.reset();
	m_enemyManager.reset();
	Sound::GetInstance().Finalize();

}

//---------------------------------------------------------
// クラスの生成
//---------------------------------------------------------
void PlayScene::CreateClass()
{
	auto deviceResources = m_commonResources->GetDeviceResources();

	// 床を作成する
	auto floor = std::make_unique<Floor>();
	floor->Initialize(m_commonResources);
	m_floors.push_back(std::move(floor));

	//フェイドを生成
	m_fade = std::make_unique<Fade>();
	m_fade->Create(deviceResources);
	// 時間のセット（フェイドアウト用）
	m_fade->SetTime(1.0f);

	//壁の生成
	m_walls = WallFactory::CreateWalls(m_commonResources, m_typemap, m_map);
	
	//坂生成
	m_collisionMesh = CollisionMeshFactory::CreateCollisionMeshs(m_commonResources,m_collisionMeshTypeMap,m_collisionMeshPosMap,m_collisionMeshRotationMap);

	// TPSカメラを作成
	m_camera = std::make_unique<Camera>();
	m_view = m_camera->GetViewMatrix();

	// プレイヤーを生成する
	m_player = std::make_unique<PlayerState>(m_camera.get(), m_walls, m_playerPos, m_floors);
	m_player->Initialize(m_commonResources);

	// 敵を生成する
	m_enemyManager = std::make_unique<EnemyManager>(m_walls, m_player.get(), m_enemyMap, m_patrolPath, m_straighteningEnemyMap,m_collisionMesh);
	m_enemyManager->Initialize(m_commonResources);

	//UIの生成
	m_ui = std::make_unique<UI>();
	m_ui->Initialize(m_commonResources);

	// 壺の配置
	for (int i = 0; i < m_vasePosMap.size(); ++i){
		m_vase.push_back(std::make_unique<Vase>(m_vasePosMap[i]));
		m_vase[i]->Initialize(m_commonResources);
	}
}

//---------------------------------------------------------
//ジェイソンの読み込み
//---------------------------------------------------------
void PlayScene::loadMapJSON(const char* filename)
{
	assert(filename);

	//そのファイルをロードする
	LoadJson loader(filename);
	//そのファイルが存在しないなら飛ばす
	if (!loader.IsLoaded()) return;
	
	// オブジェクト情報の取得
	std::vector<ObjectData> walls = loader.GetObjects("walls");
	std::vector<ObjectData> CollisionMesh = loader.GetObjects("CollisionMesh");
	std::vector<ObjectData> enemyObjectects = loader.GetObjects("EnemyPos");
	std::vector<ObjectData> enemyPath = loader.GetObjects("EnemyPath");
	std::vector<ObjectData> straighteningEnemyObjects = loader.GetObjects("StraighteningEnemyPos");
	std::vector<ObjectData> playerPos = loader.GetObjects("PlayerPos");
	std::vector<ObjectData> goalPos = loader.GetObjects("GoalPos");
	std::vector<ObjectData> vasePos = loader.GetObjects("VasePos");

	// 壁の配列
	for (const auto& object : walls) {
		m_typemap.push_back(object.type);
		m_map.push_back(object.position);
	}
	// 坂の配列と生成
	for (const auto& object : CollisionMesh){
		m_collisionMeshPosMap.push_back(object.position);
		m_collisionMeshRotationMap.push_back(object.rotation);
		m_collisionMeshTypeMap.push_back(object.type);
	}
	// 敵の配列
	for (const auto& object : enemyObjectects) {
		m_enemyMap.push_back(object.position);
	}
	// 敵のパスの配列
	for (const auto& object : enemyPath) {
		m_patrolPath.push_back(object.position);
	}
	// 突進敵の配列
	for (const auto& object : straighteningEnemyObjects) {
		m_straighteningEnemyMap.push_back(object.position);
	}
	// プレイヤーの位置
	for (const auto& object : playerPos) {
		m_playerPos = object.position;
	}
	// ゴール
	for (const auto& object : goalPos) {
		m_goal = std::make_unique<Goal>(object.position);
		m_goal->Initialize(m_commonResources);
	}
	// 壺の配列
	for (const auto& object : vasePos) {
		m_vasePosMap.push_back(object.position);
	}
	
}

//---------------------------------------------------------
// カメラの回転をする
//---------------------------------------------------------
void PlayScene::CameraRotate()
{
	// キーボードステートを取得する
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();
	// カメラ回転処理
	if (keyboardState.A){
		if (!m_isrotateLeftPressed){
			// Aキーが押された瞬間だけ回転する
			m_camera->RotateCameraAroundPlayer(DirectX::XMConvertToRadians(90.0f)); 
			// フラグをオンにする
			m_isrotateLeftPressed = true; 
		}
	}
	else{
		// Aキーが離されたらフラグをオフにする
		m_isrotateLeftPressed = false; 
	}
	if (keyboardState.D){
		if (!m_isrotateRightPressed){
			// Dキーが押された瞬間だけ回転する
			m_camera->RotateCameraAroundPlayer(-DirectX::XMConvertToRadians(90.0f)); 
			// フラグをオンにする
			m_isrotateRightPressed = true; 
		}
	}
	else{
		// Dキーが離されたらフラグをオフにする
		m_isrotateRightPressed = false; 
	}
}

//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID PlayScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene){
		return IScene::SceneID::RESULT;
	}
	// ハートが空なら
	if (m_isEmptyHart){
		//フェイドを呼び出す
		m_fade->Render(m_ui.get());
		//1.5秒以上経ったらシーンを変える
		if (m_fade->GetTime() >= 1.5f){
			return IScene::SceneID::TITLE;
		}	
	}
	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// 坂の当たり判定(プレイヤ）
//---------------------------------------------------------
void PlayScene::CheckPlayerCollisionMesh()
{
	using namespace DirectX::SimpleMath;

	//コリジョンメッシュの当たり判定情報の取得
	auto collision = mylib::Collision::CheckCollisionMesh(m_collisionMesh, m_player->GetPosition(),
		m_player->GetForwardDirection());

	// 衝突が発生した場合の処理
	if (!collision.hasHit) return;

	// 法線ベクトルを正規化
	collision.normal.Normalize();

	// プレイヤーの位置
	float playerYPosition = m_player->GetPosition().y;
	// 衝突点
	float collisionMeshYPosition = collision.hitPosition.y;
	
	// Y座標を比較 プレイヤーが坂の上の場合
	if (playerYPosition < collisionMeshYPosition){
		Vector3 pushBackPosition;
		// 押し返す
		if (collisionMeshYPosition - playerYPosition > 0.2f){
			// 押し返しの値の計算
			pushBackPosition = collision.hitPosition + collision.normal * PUSHBACKSTRENGTH;
		}
		else{
			// 坂道を登れるように少しだけ押し返す
			pushBackPosition = collision.hitPosition + collision.normal * SMALLPUSHBACKSTRENGTH;
		}
		//押し返しの値を設定する
		m_player->SetPosition(pushBackPosition);
	}
}

//---------------------------------------------------------
// 坂の当たり判定(ボム）
//---------------------------------------------------------
void PlayScene::CheckBomCollisionMesh()
{
	mylib::Collision::CollisionResult collision;

	//コリジョンメッシュの当たり判定情報の取得
	for (int i = 0; i < PlayerState::BOMMAX; i++){

		// コリジョンとボムの当たり判定
		collision = mylib::Collision::CheckCollisionMesh(m_collisionMesh, m_player->GetBomState(i)->GetPosition(), 
			m_player->GetForwardDirection());

		// 衝突が発生してないなら飛ばす
		if (!collision.hasHit) continue;

		// 法線ベクトルを正規化
		collision.normal.Normalize();

		// 衝突位置を法線方向に少し補正（めり込み防止）
		m_hitPosition += collision.normal;

		// プレイヤーのボム位置とボムの状態を更新
		m_player->GetBomState(i)->SetPosition(m_hitPosition);

		//　バウンドをしたにする
		m_player->GetBomState(i)->SetBounce(true);
	}
}

//---------------------------------------------------------
// スカイドームの描画
//---------------------------------------------------------
void PlayScene::RenderSkyModel()
{
	using namespace DirectX;

	auto states = m_commonResources->GetCommonStates();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	//スカイモデルのエフェクト情報を更新する
	m_sky->UpdateEffects([](DirectX::IEffect* effect)
		{
			// ベーシックエフェクトを設定する
			BasicEffect* basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				// 個別のライトをすべて無効化する
				basicEffect->SetLightEnabled(0, false);
				basicEffect->SetLightEnabled(1, false);
				basicEffect->SetLightEnabled(2, false);
			}
		}
	);

	//ワールド行列を更新する
	SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0, 0, 0.0f)) * DirectX::SimpleMath::Matrix::CreateScale(0.5f);

	// スカイモデルを描画する
	m_sky->Draw(context, *states, world, m_view, m_projection);
}

//---------------------------------------------------------
// リザルトチェンジ用の変数の設定
//---------------------------------------------------------
void PlayScene::RequestResultScene()
{
	// ハートが空になった
	m_isEmptyHart = true;
	// フェイド用の時間設定
	m_fade->SetTime(0.0f);
}
