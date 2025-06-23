/*
  @file  PlayerState.cpp
  @brief �v���C����ԃN���X
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
// �R���X�g���N�^
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
	m_isThrow{ false }, // ture�@������
	m_bomExist{ false }

{

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
PlayerState::~PlayerState()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void PlayerState::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;
	//�o�E�f�B���O�{�b�N�X�̍쐬
	m_boundingBox.Center = m_position;
	m_boundingBox.Extents = m_extentsScale;
	//�v���C���[�Ɋۉe��ǉ�����
	InitializeShadow();
	//�{����ǉ�����
	InitializeBom();
	//�X�e�[�g��ǉ�����
	InitializeStates();

#ifdef _DEBUG	// �f�o�b�N�����s
	//�f�o�b�O�p�̓����蔻���ǉ�
	InitializeCollisionDebugRenderer();
#endif

}

//---------------------------------------------------------
// �V������ԂɑJ�ڂ���
//---------------------------------------------------------
void PlayerState::ChangeState(IPlayerState* newPlayerState)
{
	// �V�K�̏�ԑJ�ڑO�Ɏ���X�V���s��
	m_currentState->PostUpdate();
	// �V�K�̏�Ԃ����݂̏�Ԃɐݒ肷��
	m_currentState = newPlayerState;
	// �V�K�̏�ԑJ�ڌ�Ɏ��O�X�V���s��
	m_currentState->PreUpdate();
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void PlayerState::Update(const float& elapsedTime)
{
	// �d�͂�������
	m_position.y -= GRVITY * elapsedTime; 
	//�v���C���[�̍X�V
	m_currentState->Update(elapsedTime);
	// �o�E�f�B���O�{�b�N�X�̍X�V
	m_boundingBox.Center = m_position;
	//�ǂƂ̓����蔻��
	CheckCollisionWithWall();
	//���Ƃ̓����蔻��
	CheckCollisionWithFloor();
	// �L�[�֌W
	CheckKeyboard();
	// ���ݎ����Ă���{���𓊂���
	if(IsThroeableBom()){
		// ������
		m_isThrow = true;
		// �A�C�h�����O��ԂɈڍs
		ChangeState(m_playerIdling.get());
	}
	//�{���̍X�V
	for (auto& bom : m_bom){
		//�{���̍X�V
		bom->Update(elapsedTime);
		//������Ԃ̕��ɂ�������
		if (mylib::Collision::BoundingCheckCollision(GetBoundingBox(), bom->GetBoundingSphere()) &&
			bom->GetBomPresent() == IBomState::BomtPresent::EXPLOSION)CheckBom();
	}
	// �o�E�f�B���O�{�b�N�X�̍X�V
	m_boundingBox.Center = m_position;
	// �v���C���[���G�ɐG�ꂽ��
	this->EnemyHit(elapsedTime);
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void PlayerState::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& projection)
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	//�`��
	m_currentState->Render(view, projection);
	//�{���̕`��
	for (auto& bom : m_bom){
		bom->Render(view, projection);
	}
	// ���@�̉e��`�悷��
	m_shadow->Render(context, states, view, projection, m_position );

#ifdef _DEBUG
	// �f�o�b�O�����uDebugString�v�ŕ\������
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("");
	debugString->AddString("PlayerpositionX : %f", m_position.x);
	debugString->AddString("PlayerpositionY : %f", m_position.y);
	debugString->AddString("PlayerpositionZ : %f", m_position.z);
	debugString->AddString("PlayerpositionCX : %f", m_boundingBox.Center.x);
	debugString->AddString("PlayerpositionCY : %f", m_boundingBox.Center.y);
	debugString->AddString("PlayerpositionCZ : %f", m_boundingBox.Center.z);
	debugString->AddString("");

	//�R���W�����̐F
	const DirectX::SimpleMath::Vector4 color(DirectX::Colors::Black.f[0],
		DirectX::Colors::Red.f[1],
		DirectX::Colors::Red.f[2],
		DirectX::Colors::Red.f[3]);
	//�f�o�b�O�`��
	m_collisionDebugRenderer->RenderBoundingBox(m_boundingBox, view, projection, color);
#endif

}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void PlayerState::Finalize()
{
}

//---------------------------------------------------------
// ���Ƃ̓����蔻��
//---------------------------------------------------------
void PlayerState::CheckCollisionWithFloor()
{
	for (auto& floor : m_floors){
		// �Փ˔���
		if (!mylib::Collision::BoundingCheckBoxCollision(m_boundingBox, floor->GetBoundingBox())) continue;

		// ����Y�����擾
		float floorTopY = floor->GetPosition().y;

		// �v���C���[������艺�ɂ���ꍇ�̂ݐڒn����
		if (m_position.y < floorTopY + 1.50f){
			// ���̏�ɌŒ�
			m_position.y = floorTopY + 1.00f; 
			return; 
		}
	}
}

//---------------------------------------------------------
// �ǂƂ̓����蔻��
//---------------------------------------------------------
void PlayerState::CheckCollisionWithWall()
{
	for (const auto& wall : m_wall)
	{
		//�ǂƂ̓����蔻������āA�������i�[����
		m_pushBack = wall->CheckHitAndResolve(m_boundingBox, wall->GetBoundingBox());

		// �����߂������������ꍇ�̂݃|�W�V�������X�V
		if (std::abs(m_pushBack.Length()) > 0.00f || std::abs(m_pushBack.Length()) < 0.00f)
		{
			m_position += m_pushBack;
		}
	}
}

//---------------------------------------------------------
// �L�[�֌W
//---------------------------------------------------------
void PlayerState::CheckKeyboard()
{
	// �L�[�{�[�h�X�e�[�g�g���b�J�[���擾����
	const auto& kbTracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	if (kbTracker->pressed.X)
	{
		auto it = std::find_if(m_bom.begin(), m_bom.end(),
			[](const auto& bom) { return !bom->GetExist(); });

		if (it != m_bom.end())
		{
			(*it)->SetExist(true);
			ChangeState(m_playerBomHand.get());
			// �C���f�b�N�X���擾
			m_index = static_cast<int>(std::distance(m_bom.begin(), it));
			m_isThrow = false;
		}
	}
}

//---------------------------------------------------------
//�G�Ɠ��������Ȃ�
//---------------------------------------------------------
void PlayerState::EnemyHit(const float& elapsedTime)
{
	// �Փˎ��̃^�C�}�[���X�V
	if (m_isEnemyHit)
	{
		m_exist = false;
		m_hitTimer += elapsedTime;

		// �_�ł̂��߂̃^�C�}�[���X�V
		m_flashTimer += elapsedTime;

		// �_�ŏ�Ԃ�؂�ւ���i0.1�b���Ƃɐ؂�ւ��j
		if (m_flashTimer >= 0.1f)
		{
			m_isFlashing = !m_isFlashing;
			m_flashTimer = 0.0f;
		}

		// 3�b�o�߂����猳�ɖ߂�
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
// �ۉe�̏�����
//---------------------------------------------------------
void PlayerState::InitializeShadow()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	

	m_shadow = std::make_unique<Shadow>();
	m_shadow->Initialize(device, context);
}

//---------------------------------------------------------
// �{���̏�����
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
// �v���C���[�̏�Ԃ̏�����
//---------------------------------------------------------
void PlayerState::InitializeStates()
{
	// �҂��Ă�����
	m_playerIdling = std::make_unique<PlayerIdling>(this, m_wall);
	m_playerIdling->Initialize(m_commonResources);
	//�@�{���������Ă�����
	m_playerBomHand = std::make_unique<PlayerBomHand>(m_camera, this, m_wall);
	m_playerBomHand->Initialize(m_commonResources);
	// �����Ă�����
	m_playerMovement = std::make_unique<PlayerMovement>(m_camera, this, m_wall);
	m_playerMovement->Initialize(m_commonResources);
	// ���݂̏��
	m_currentState = m_playerIdling.get();
}

//---------------------------------------------------------
// �f�o�b�N�R���W�����̏�����
//---------------------------------------------------------
void PlayerState::InitializeCollisionDebugRenderer()
{
	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	//�f�o�b�N�p�̃R���W����
	m_collisionDebugRenderer = std::make_unique<CollisionDebugRenderer>(device, context);
}

//---------------------------------------------------------
//�����Ɠ�������Ȃ���Ԃ����ւ���
//---------------------------------------------------------
void PlayerState::EnemyHitFlashing(const DirectX::Model& model, const DirectX::FXMMATRIX world, const  DirectX::CXMMATRIX view, const  DirectX::CXMMATRIX projection)
{
	using namespace DirectX::SimpleMath;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// ���������ʂ̂��߂̃u�����h�X�e�[�g
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

	// �[�x�o�b�t�@�̐ݒ�i�������p�j
	ID3D11DepthStencilState* depthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // �[�x�o�b�t�@�̍X�V�𖳌��ɂ���
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	m_commonResources->GetDeviceResources()->GetD3DDevice()->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	// �������̕`�悪�K�v�ȏꍇ
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
				context->OMSetDepthStencilState(states->DepthDefault(), 0);	// �Z
			}
		);
	}
	// �u�����h�X�e�[�g�Ɛ[�x�X�e���V���X�e�[�g�����ɖ߂�
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
//�{���ƐG�ꂽ���̌���
//---------------------------------------------------------
void PlayerState::CheckBom()
{
	m_exist = false;
	SetHitEnemy(true);
}

//---------------------------------------------------------
//�@�{����������Ă��邩
//---------------------------------------------------------
bool PlayerState::IsThroeableBom()
{
	auto bomState = GetBomState(m_index)->GetBomPresent();
	return (bomState == IBomState::MOVEMENT || bomState == IBomState::STAY) && !m_isThrow;
}
