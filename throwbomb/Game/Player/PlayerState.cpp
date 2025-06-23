/*
  @file  PlayerState.cpp
  @brief プレイヤ状態クラス
*/
#include "pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/CollisionDebugRenderer.h"
#include "Game/Player/PlayerState.h"
#include "Game/Player/State/PlayerIdling.h"
#include "Game/Player/State/PlayerBomHand.h"
#include "Game/Player/State/PlayerMovement.h"
#include "Game/Bom/BomState.h"
#include "Game/Bom/State/BomExist.h"
#include "Game/Wall/Wall.h"
#include "Game/Shadow/Shadow.h"
#include "Game/Collision/Collision.h"
#include "Game/Floor/Floor.h"


//---------------------------------------------------------
// コンストラクタ
//---------------------------------------------------------
PlayerState::PlayerState(Camera* camera, const std::vector<std::unique_ptr<Wall>>& wall, DirectX::SimpleMath::Vector3 pos, std::vector<std::shared_ptr<Floor>>& floors)
	: 
	m_camera(camera),
	m_wall(wall),
	m_position(pos),
	m_extentsScale(0.5f, 0.5f, 0.5f),
	m_target(DirectX::SimpleMath::Vector3::Zero),
	m_rotate(DirectX::SimpleMath::Quaternion::FromToRotation(DirectX::SimpleMath::Vector3::UnitZ, -DirectX::SimpleMath::Vector3::UnitZ)),
	m_exist(true),
	m_isHitWall(false),
	m_isHitFloor(false),
	m_isEnemyHit(false),
	m_isFlashing(false),
	m_hitTimer(0.0f),
	m_flashTimer(0.0f),
	m_boundingBox({}),
	m_commonResources(nullptr),
	m_currentState(nullptr),
	m_floors{ floors },
	m_pushBack{ DirectX::SimpleMath::Vector3::Zero },
	m_shadow{ nullptr },
	m_index{ 0 },
	m_isThrow{ false }, // ture　投げた
	m_bomExist{ false }

{

}

//---------------------------------------------------------
// デストラクタ
//---------------------------------------------------------
PlayerState::~PlayerState()
{
}

//---------------------------------------------------------
// 初期化する
//---------------------------------------------------------
void PlayerState::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	//バウディングボックスの作成
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = m_extentsScale;
	//プレイヤーに丸影を追加する
	InitializeShadow();
	//ボムを追加する
	InitializeBom();
	//ステートを追加する
	InitializeStates();

#ifdef _DEBUG	// デバック時実行
	//デバッグ用の当たり判定を追加
	InitializeCollisionDebugRenderer();
#endif

}

//---------------------------------------------------------
// 新しい状態に遷移する
//---------------------------------------------------------
void PlayerState::ChangeState(IPlayerState* newPlayerState)
{
	// 新規の状態遷移前に事後更新を行う
	m_currentState->PostUpdate();
	// 新規の状態を現在の状態に設定する
	m_currentState = newPlayerState;
	// 新規の状態遷移後に事前更新を行う
	m_currentState->PreUpdate();
}

//---------------------------------------------------------
// 更新する
//---------------------------------------------------------
void PlayerState::Update(const float& elapsedTime)
{
	// 重力を加える
	m_position.y -= GRVITY * elapsedTime; 
	//プレイヤーの更新
	m_currentState->Update(elapsedTime);
	// バウディングボックスの更新
	m_boundingBox.Center = m_position;
	//壁との当たり判定
	CheckCollisionWithWall();
	//床との当たり判定
	CheckCollisionWithFloor();
	// キー関係
	CheckKeyboard();
	// 現在持っているボムを投げる
	if(IsThroeableBom()){
		// 投げる
		m_isThrow = true;
		// アイドリング状態に移行
		ChangeState(m_playerIdling.get());
	}
	//ボムの更新
	for (auto& bom : m_bom){
		//ボムの更新
		bom->Update(elapsedTime);
		//爆発状態の物にあったら
		if (mylib::Collision::BoundingCheckCollision(GetBoundingBox(), bom->GetBoundingSphere()) &&
			bom->GetBomPresent() == IBomState::BomtPresent::EXPLOSION)CheckBom();
	}
	// バウディングボックスの更新
	m_boundingBox.Center = m_position;
	// プレイヤーが敵に触れた時
	this->EnemyHit(elapsedTime);
}

//---------------------------------------------------------
// 描画する
//---------------------------------------------------------
void PlayerState::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	//描画
	m_currentState->Render(view, projection);
	//ボムの描画
	for (auto& bom : m_bom){
		bom->Render(view, projection);
	}
	// 自機の影を描画する
	m_shadow->Render(context, states, view, projection, m_position );

#ifdef _DEBUG
	// デバッグ情報を「DebugString」で表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("");
	debugString->AddString("PlayerpositionX : %f", m_position.x);
	debugString->AddString("PlayerpositionY : %f", m_position.y);
	debugString->AddString("PlayerpositionZ : %f", m_position.z);
	debugString->AddString("PlayerpositionCX : %f", m_boundingBox.Center.x);
	debugString->AddString("PlayerpositionCY : %f", m_boundingBox.Center.y);
	debugString->AddString("PlayerpositionCZ : %f", m_boundingBox.Center.z);
	debugString->AddString("");

	//コリジョンの色
	const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
		DirectX::Colors::Red.f[1],
		DirectX::Colors::Red.f[2],
		DirectX::Colors::Red.f[3]);
	//デバッグ描画
	m_collisionDebugRenderer->RenderBoundingBox(m_boundingBox, view, projection, color);
#endif

}

//---------------------------------------------------------
// 後始末する
//---------------------------------------------------------
void PlayerState::Finalize()
{
}

//---------------------------------------------------------
// 床との当たり判定
//---------------------------------------------------------
void PlayerState::CheckCollisionWithFloor()
{
	for (auto& floor : m_floors){
		// 衝突判定
		if (!mylib::Collision::BoundingCheckBoxCollision(m_boundingBox, floor->GetBoundingBox())) continue;

		// 床のY軸を取得
		float floorTopY = floor->GetPosition().y;

		// プレイヤーが床より下にいる場合のみ接地判定
		if (m_position.y < floorTopY + 1.50f){
			// 床の上に固定
			m_position.y = floorTopY + 1.00f; 
			return; 
		}
	}
}

//---------------------------------------------------------
// 壁との当たり判定
//---------------------------------------------------------
void PlayerState::CheckCollisionWithWall()
{
	for (const auto& wall : m_wall)
	{
		//壁との当たり判定をして、差分を格納する
		m_pushBack = wall->CheckHitAndResolve(m_boundingBox, wall->GetBoundingBox());

		// 押し戻しが発生した場合のみポジションを更新
		if (std::abs(m_pushBack.Length()) > 0.00f || std::abs(m_pushBack.Length()) < 0.00f)
		{
			m_position += m_pushBack;
		}
	}
}

//---------------------------------------------------------
// キー関係
//---------------------------------------------------------
void PlayerState::CheckKeyboard()
{
	// キーボードステートトラッカーを取得する
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	if (kbTracker->pressed.X)
	{
		auto it = std::find_if(m_bom.begin(), m_bom.end(),
			[](const auto& bom) { return !bom->GetExist(); });

		if (it != m_bom.end())
		{
			(*it)->SetExist(true);
			ChangeState(m_playerBomHand.get());
			// インデックスを取得
			m_index = static_cast<int>(std::distance(m_bom.begin(), it));
			m_isThrow = false;
		}
	}
}

//---------------------------------------------------------
//敵と当たったなら
//---------------------------------------------------------
void PlayerState::EnemyHit(const float& elapsedTime)
{
	// 衝突時のタイマーを更新
	if (m_isEnemyHit)
	{
		m_exist = false;
		m_hitTimer += elapsedTime;

		// 点滅のためのタイマーを更新
		m_flashTimer += elapsedTime;

		// 点滅状態を切り替える（0.1秒ごとに切り替え）
		if (m_flashTimer >= 0.1f)
		{
			m_isFlashing = !m_isFlashing;
			m_flashTimer = 0.0f;
		}

		// 3秒経過したら元に戻す
		if (m_hitTimer >= 3.0f)
		{
			m_isEnemyHit = false;
			m_isFlashing = false;
			m_hitTimer = 0.0f;
			m_flashTimer = 0.0f;
		}
	}
	else
	{
		m_exist = true;
	}
}

void PlayerState::PreUpdate()
{
}

void PlayerState::PostUpdate()
{
}

//---------------------------------------------------------
// 丸影の初期化
//---------------------------------------------------------
void PlayerState::InitializeShadow()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	

	m_shadow = std::make_unique<Shadow>();
	m_shadow->Initialize(device, context);
}

//---------------------------------------------------------
// ボムの初期化
//---------------------------------------------------------
void PlayerState::InitializeBom()
{
	for (int i = 0; i < BOMMAX; i++)
	{
		auto bom = std::make_unique<BomState>(this, m_wall);
		bom->Initialize(m_commonResources);
		m_bom.push_back(std::move(bom));
	}
}

//---------------------------------------------------------
// プレイヤーの状態の初期化
//---------------------------------------------------------
void PlayerState::InitializeStates()
{
	// 待っている状態
	m_playerIdling = std::make_unique<PlayerIdling>(this, m_wall);
	m_playerIdling->Initialize(m_commonResources);
	//　ボムを持っている状態
	m_playerBomHand = std::make_unique<PlayerBomHand>(m_camera, this, m_wall);
	m_playerBomHand->Initialize(m_commonResources);
	// 動いている状態
	m_playerMovement = std::make_unique<PlayerMovement>(m_camera, this, m_wall);
	m_playerMovement->Initialize(m_commonResources);
	// 現在の状態
	m_currentState = m_playerIdling.get();
}

//---------------------------------------------------------
// デバックコリジョンの初期化
//---------------------------------------------------------
void PlayerState::InitializeCollisionDebugRenderer()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	//デバック用のコリジョン
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
}

//---------------------------------------------------------
//透明と透明じゃない状態を入れ替える
//---------------------------------------------------------
void PlayerState::EnemyHitFlashing(const DirectX::Model& model, const DirectX::FXMMATRIX world, const  DirectX::CXMMATRIX view, const  DirectX::CXMMATRIX projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// 半透明効果のためのブレンドステート
	ID3D11BlendState* blendState = nullptr;
	if (m_isEnemyHit && m_isFlashing)
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		m_commonResources->GetDeviceResources()->GetD3DDevice()->CreateBlendState(&blendDesc, &blendState);
	}

	// 深度バッファの設定（半透明用）
	ID3D11DepthStencilState* depthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 深度バッファの更新を無効にする
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	m_commonResources->GetDeviceResources()->GetD3DDevice()->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	// 半透明の描画が必要な場合
	if (m_isEnemyHit && m_isFlashing)
	{
		context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
		context->OMSetDepthStencilState(depthStencilState, 1);
		model.Draw(
			context,
			*states,
			world,
			view,
			projection,
			false,
			[&]()	
			{
				context->OMSetDepthStencilState(states->DepthDefault(), 0);	// 〇
			}
		);
	}
	// ブレンドステートと深度ステンシルステートを元に戻す
	if (blendState)
	{
		context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
		blendState->Release();
	}
	if (depthStencilState)
	{
		context->OMSetDepthStencilState(nullptr, 1);
		depthStencilState->Release();
	}
}

//---------------------------------------------------------
//ボムと触れた時の効果
//---------------------------------------------------------
void PlayerState::CheckBom()
{
	m_exist = false;
	SetHitEnemy(true);
}

//---------------------------------------------------------
//　ボムを手放しているか
//---------------------------------------------------------
bool PlayerState::IsThroeableBom()
{
	auto bomState = GetBomState(m_index)->GetBomPresent();
	return (bomState == IBomState::MOVEMENT || bomState == IBomState::STAY) && !m_isThrow;
}
