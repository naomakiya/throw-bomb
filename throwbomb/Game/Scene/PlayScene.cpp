/*
	@file	PlayScene.cpp
	@brief	プレイシーンクラス
*/
#include "pch.h"
#include "PlayScene.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/InputManager.h"
#include "Game/Floor/Floor.h"
#include "Game/Camera/Camera.h"
#include "Game/Collision/Collision.h"
#include "Game/UI/UI.h"
#include "Game/Factory/EnemyFactory.h"
#include "Game/Factory/StraighteningEnemyFactory.h"
#include "Game//Factory/WallFactory.h"
#include "Game/Goal/Goal.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/BomState.h"
#include "Game/UI/Fade.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Game/Vase/Vase.h"
#include "Game/Collision/Collision.h"
#include "Libraries/MyLib/CollisionMesh.h"
#include "Game/Sound/Sound.h"
#include "Game/Enemy/EnemyManager.h"
#include"Game/UI/Number/Number.h"
#include "SceneManager.h"
#include "Game/Factory/CollisionMeshFactory.h"
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
	m_se{},
	m_map{},
	m_fade{},
	m_vase{},
	m_hitPolygonNo{},
	m_enemyMap{},
	m_typemap{},
	m_Wallcrack{},
	m_iseEmptyHart{false},
	m_enemyModel{},
	m_straighteningEnemyMap{},
	m_enemyManager{},
	m_elapsedTime{},
	m_seVolume{},
	m_bgmVolume{},
	m_collisionMeshPosMap{},
	m_collisionMeshRotationMap{},
	m_collisionMeshTypeMap{}
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
	//　セレクトナンバーの選択
	int number = m_sceneManager->GetSelectNumber();
	// int を std::string に変換
	std::string numberString = std::to_string(number);
	// マップ情報をJSONファイルから読み込んで作成
	loadMapJSON(ResourceManager::getStagePath(numberString).c_str());

	// 床を作成する
	DirectX::SimpleMath::Vector3 position = { 0, -0.0f, 0 };
	float floorWidth = 100.0f;
	float floorDepth = 100.0f;

	auto floor = std::make_unique<Floor>(device, position, floorWidth, floorDepth);
	floor->Initialize(device, m_commonResources);
	m_floors.push_back(std::move(floor));

	// class生成
	ClassCreate();

	//読み込み準備
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//スカイドーム
	m_sky = Model::CreateFromCMO(device, ResourceManager::getModelPath("SkyDome").c_str(), *fx);

	LoadJson json("Resources/Json/Music.json");

	m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
	m_seVolume = json.GetJson()["SE"].value("Volume", 0.0f);

	//サウンドのインスタンスの獲得
	Sound::GetInstance().Initialize();
	Sound::GetInstance().PlayBGM(ResourceManager::getBGMPath("PlayBGM").c_str(), true);
	Sound::GetInstance().SetVolume(m_bgmVolume);
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);
	m_elapsedTime = elapsedTime;
	
	// プレイヤーを更新する
	m_player->Update(elapsedTime);
	//Uiの更新
	for (int i = 0; i < 3; i++)
	{
		m_ui->Update();
	}
	//カメラの回転キー
	CameraRotate();
	//カメラ更新
	m_camera->UpdateCameraRotation(elapsedTime);
	// カメラの位置をプレイヤーの位置に連動させる
	m_camera->Update(m_player->GetPosition());
	m_view = m_camera->GetViewMatrix();
	//敵の更新
	m_enemyManager->Update(elapsedTime);
	//Rayを使用してコリジョンメッシュと衝突判定をとる(プレイヤー）
	CollisionMeshDeterminationPlayer();

	//Rayを使用してコリジョンメッシュと衝突判定をとる(ボム）
	for (int i = 0; i < 3; i++)
	{
		if (m_player->GetBomState(i)->GetExist())
		{	
			CollisionMeshDeterminationBom();
		}
	}
	//壺とプレイヤーの当たり判定
	for (auto& vase : m_vase)
	{

		if (vase->GetExist())
		{
			m_player->SetPosition(m_player->GetPosition() - mylib::Collision::CheckHit(vase->GetBoundingBox(), m_player->GetBoundingBox()));

			for (int i = 0; i < 3; i++)
			{
				// 爆発状態の時に触れたら無くす
				if (m_player->GetBomState(i)->GetBomPresent() == BomState::BomtPresent::EXPLOSION) continue;

				if (mylib::Collision::BoundingCheckCollision(vase->GetBoundingBox(), m_player->GetBomState(i)->GetBoundingSphere()))
				{
					vase->SetExist(false);
				}

			}
		}
		else
		{
			vase->BreacVase();

			if (vase->GetBoundingSphere().Radius < 15.0)
			{
				m_enemyManager->SetVacePos(vase->GetPosition());
			}
		}
	}

	// プレイヤーが敵にヒットしたかどうかを確認
	if (!m_player->GetExist() && !m_canCountHeart){
		m_ui->CountHeart();
		m_canCountHeart = true;     // 一度だけ呼び出すためにフラグをオフにする
		m_heartCooldownTimer = 0.0f; // クールダウンタイマーをリセット
	}

	// クールダウンタイマーを更新
	if (m_canCountHeart)
	{
		//時間を計測する
		m_heartCooldownTimer += elapsedTime;

		// 3秒後に再び呼び出せるようにフラグをオンにする
		if (m_heartCooldownTimer >= 3.0f){
			m_canCountHeart = false;
		}
	}

	// ゴールに触れた場合はシーンを変更
	if (mylib::Collision::BoundingCheckCollision(m_player->GetBoundingBox(), m_goal->GetBoundingSphere())){
		m_isChangeScene = true;
		m_fade->SetTime(0.0f);
	}

	// ハートが全て空になったらリザルトシーンに遷移する
	if (m_ui->AreHeartsEmpty() && !m_iseEmptyHart){
		m_iseEmptyHart = true;
		m_fade->SetTime(0.0f);
	}

	if (m_ui->GetNumber()->GetTime() <= 0.0f && !m_iseEmptyHart){
		m_iseEmptyHart = true;
		m_fade->SetTime(0.0f);
	}

	//敵が全滅したらクリア
	if (m_enemyManager->GetEmpty() && !m_isChangeScene){
		m_isChangeScene = true;
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
		floor->Render(context, m_view, m_projection);
	}

	// 壁の描画
	for (const auto& wall : m_walls){	
		// 表示されているなら
		if (!wall->IsVisible())wall->Render(context, m_view, m_projection);
	}

	// メッシュを描画する
	for (auto& collisionMesh : m_collisionMesh){
		collisionMesh->Draw(context, states, m_view, m_projection);
	}
	// 壺の描画
	for (auto& vase : m_vase)
	{
		if (vase->GetExist()) vase->Render(context, m_view, m_projection);
	}
	//敵の描画
	m_enemyManager->Render(context, m_view, m_projection);
	// プレイヤーを描画する
	m_player->Render(m_view, m_projection);
	//スカイモデルの描画
	SkyModelRender();
	//ゴールの描画
	m_goal->Render(context, m_view, m_projection);

	//UIの描画
	m_ui->Render(m_elapsedTime);

	//フェイドの表示
	if (m_fade->GetTime() < 3.25f)
	{
		m_fade->Timer(m_elapsedTime);
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
void PlayScene::ClassCreate()
{
	auto deviceResources = m_commonResources->GetDeviceResources();

	//フェイドを生成
	m_fade = std::make_unique<Fade>();
	m_fade->Create(deviceResources);
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
	for (int i = 0; i < m_vasePosMap.size(); ++i)
	{
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
	/*auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();*/

	//そのファイルをロードする
	LoadJson loader(filename);
	//そのファイルが存在しないなら飛ばす
	if (!loader.IsLoaded()) return;
	
	std::vector<ObjectData> walls = loader.GetObjects("walls");
	std::vector<ObjectData> CollisionMesh = loader.GetObjects("CollisionMesh");
	std::vector<ObjectData> enemyObjects = loader.GetObjects("EnemyPos");
	std::vector<ObjectData> enemyPath = loader.GetObjects("EnemyPath");
	std::vector<ObjectData> straighteningEnemyObjects = loader.GetObjects("StraighteningEnemyPos");
	std::vector<ObjectData> playerPos = loader.GetObjects("PlayerPos");
	std::vector<ObjectData> goalPos = loader.GetObjects("GoalPos");
	std::vector<ObjectData> vasePos = loader.GetObjects("VasePos");

	// 壁の配列
	for (const auto& obj : walls) {
		m_typemap.push_back(obj.type);
		m_map.push_back(obj.position);
	}
	// 坂の配列と生成
	for (const auto& obj : CollisionMesh)
	{
		m_collisionMeshPosMap.push_back(obj.position);
		m_collisionMeshRotationMap.push_back(obj.rotation);
		m_collisionMeshTypeMap.push_back(obj.type);
	}
	for (const auto& obj : enemyObjects) {
		m_enemyMap.push_back(obj.position);
	}
	for (const auto& obj : enemyPath) {
		m_patrolPath.push_back(obj.position);
	}
	for (const auto& obj : straighteningEnemyObjects) {
		m_straighteningEnemyMap.push_back(obj.position);
	}
	for (const auto& obj : playerPos) {
		m_playerPos = obj.position;
	}
	for (const auto& obj : goalPos) {
		//ゴール生成
		m_goal = std::make_unique<Goal>(obj.position);
		m_goal->Initialize(m_commonResources);
	}
	for (const auto& obj : vasePos) {
		m_vasePosMap.push_back(obj.position);
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
	if (keyboardState.A)
	{
		if (!rotateLeftPressed)
		{ // Aキーが押された瞬間だけ回転する
			m_camera->RotateCameraAroundPlayer(DirectX::XMConvertToRadians(90.0f)); // 左に90度回転
			rotateLeftPressed = true; // フラグをオンにする
		}
	}
	else
	{
		rotateLeftPressed = false; // Aキーが離されたらフラグをオフにする
	}
	if (keyboardState.D)
	{
		if (!rotateRightPressed)
		{ // Dキーが押された瞬間だけ回転する
			m_camera->RotateCameraAroundPlayer(-DirectX::XMConvertToRadians(90.0f)); // 右に90度回転
			rotateRightPressed = true; // フラグをオンにする
		}
	}
	else
	{
		rotateRightPressed = false; // Dキーが離されたらフラグをオフにする
	}

}



//---------------------------------------------------------
// 次のシーンIDを取得する
//---------------------------------------------------------
IScene::SceneID PlayScene::GetNextSceneID() const
{
	// シーン変更がある場合
	if (m_isChangeScene)
	{
		return IScene::SceneID::RESULT;
	}
	if (m_iseEmptyHart == true)
	{
		//フェイドを呼び出す
		m_fade->Render(m_ui.get());
		//1秒以上経ったらシーンを変える
		if (m_fade->GetTime() >= 1.5f)
		{
			return IScene::SceneID::TITLE;
		}
		
	}
	// シーン変更がない場合
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// 坂の当たり判定(プレイヤ）
//---------------------------------------------------------
void PlayScene::CollisionMeshDeterminationPlayer()
{
	using namespace DirectX::SimpleMath;

	//コリジョンメッシュの当たり判定情報の取得
	auto collision = mylib::Collision::CheckCollisionMesh(m_collisionMesh, m_player->GetPosition(), m_player->GetForwardDirection());

	// 衝突が発生した場合の処理
	if (!collision.hasHit) return;
	
	collision.normal.Normalize();

	// プレイヤーの位置と衝突点のY座標を比較
	float playerYPosition = m_player->GetPosition().y;
	float collisionMeshYPosition = collision.hitPosition.y;

	// プレイヤーが坂の上 
	if (playerYPosition < collisionMeshYPosition)
	{
		Vector3 pushBackPosition;

		// 強く押し返す場合
		if (collisionMeshYPosition - playerYPosition > 0.2f)
		{
			float pushBackStrength = 0.5f;
			pushBackPosition = collision.hitPosition + collision.normal * pushBackStrength;
		}
		else
		{
			// 坂道を登れるように少しだけ押し返す
			float smallPushBackStrength = 0.01f;
			pushBackPosition = collision.hitPosition + collision.normal * smallPushBackStrength;
		}

		m_player->SetPosition(pushBackPosition);
	}
}



//---------------------------------------------------------
// 坂の当たり判定(ボム）
//---------------------------------------------------------
void PlayScene::CollisionMeshDeterminationBom()
{
	mylib::Collision::CollisionResult collision;

	//コリジョンメッシュの当たり判定情報の取得
	for (int i = 0; i < 3; i++)
	{
		collision = mylib::Collision::CheckCollisionMesh(m_collisionMesh, m_player->GetBomState(i)->GetPosition(), m_player->GetForwardDirection());

		// 衝突が発生してないなら飛ばす
		if (!collision.hasHit) continue;

		// 法線ベクトルを正規化
		collision.normal.Normalize();

		// 衝突位置を法線方向に少し補正（めり込み防止）
		m_hitPosition += collision.normal;

		// プレイヤーのボム位置とボムの状態を更新
		m_player->GetBomState(i)->SetPosition(m_hitPosition);

		m_player->GetBomState(i)->SetBounce(true);
	}

}


//---------------------------------------------------------
// スカイドームの描画
//---------------------------------------------------------
void PlayScene::SkyModelRender()
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
