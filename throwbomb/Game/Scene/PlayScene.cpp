/*
	@file	PlayScene.cpp
	@brief	�v���C�V�[���N���X
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

//nlohmann/json.hpp���C�u����
#include"nlohmann/json.hpp"
#include <Framework/LoadJson.h>
using json = nlohmann::json;

//---------------------------------------------------------
// �R���X�g���N�^
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
// �f�X�g���N�^
//---------------------------------------------------------
PlayScene::~PlayScene()
{
	Finalize();
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void PlayScene::Initialize(CommonResources* resources)
{
	using namespace DirectX;

	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

	// �f�o�b�O�J�������쐬����
	RECT rect{ m_commonResources->GetDeviceResources()->GetOutputSize() };

	// �ˉe�s����쐬����
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.15f, 1000.0f
	);


	//�@�X�e�[�W�ԍ��̎擾
	int number = m_sceneManager->GetSelectNumber();

	// int �� std::string �ɕϊ�
	std::string numberString = std::to_string(number);

	// �}�b�v����JSON�t�@�C������ǂݍ���ō쐬
	loadMapJSON(ResourceManager::getStagePath(numberString).c_str());

	// �N���X����
	CreateClass();


	//�ǂݍ��ݏ���
	std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
	fx->SetDirectory(L"Resources/Models");
	//�X�J�C�h�[��
	m_sky = Model::CreateFromCMO(device, ResourceManager::getModelPath("SkyDome").c_str(), *fx);


	// ���ʂ̓ǂݍ���
	LoadJson json("Resources/Json/Music.json");
	// BGM�̉��ʂ̐ݒ�
	m_bgmVolume = json.GetJson()["BGM"].value("Volume", 0.0f);
	//�@SE�̉��ʂ̐ݒ�
	m_seVolume = json.GetJson()["SE"].value("Volume", 0.0f);
	// �T�E���h�̃C���X�^���X�̊l��
	Sound::GetInstance().Initialize();
	// BGM�̍Đ�
	Sound::GetInstance().PlayBGM(ResourceManager::getBGMPath("PlayBGM").c_str(), true);
	// ���ʂ̐ݒ�
	Sound::GetInstance().SetVolume(m_bgmVolume);
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void PlayScene::Update(float elapsedTime)
{
	UNREFERENCED_PARAMETER(elapsedTime);

	//���Ԍo��
	m_elapsedTime = elapsedTime;
	
	// �v���C���[���X�V����
	m_player->Update(elapsedTime);

	//Ui�̍X�V
	m_ui->Update();

	//�J�����̉�]�L�[
	CameraRotate();

	//�J�����X�V
	m_camera->UpdateCameraRotation(elapsedTime);

	// �J�����̈ʒu���v���C���[�̈ʒu�ɘA��������
	m_camera->Update(m_player->GetPosition());

	// �r���[�̏��̎擾
	m_view = m_camera->GetViewMatrix();

	//�G�̍X�V
	m_enemyManager->Update(elapsedTime);

	//Ray���g�p���ăR���W�������b�V���ƏՓ˔�����Ƃ�(�v���C���[�j
	CheckPlayerCollisionMesh();

	//Ray���g�p���ăR���W�������b�V���ƏՓ˔�����Ƃ�(�{���j
	for (int i = 0; i < PlayerState::BOMMAX; i++){
		//	�{�����L��̂Ȃ画�������
		if (m_player->GetBomState(i)->GetExist()){	
			CheckBomCollisionMesh();
		}
	}

	//��ƃv���C���[�̓����蔻��
	for (auto& vase : m_vase){
		// �₪�L�邩�H
		if (vase->GetExist()){
			// �Փ˔���̌��ʂ��v���C���ɐݒ肷��
			m_player->SetPosition(m_player->GetPosition() - 
				mylib::Collision::CheckHit(vase->GetBoundingBox(), m_player->GetBoundingBox()));
			//�{���̔��菈��
			for (int i = 0; i < PlayerState::BOMMAX; i++){
				// ������ԏo�Ȃ��Ȃ��΂�
				if (m_player->GetBomState(i)->GetBomPresent() == BomState::BomtPresent::EXPLOSION) continue;
				// �����ƚ₪�������Ă��邩
				if (mylib::Collision::BoundingCheckCollision(vase->GetBoundingBox(),
					m_player->GetBomState(i)->GetBoundingSphere())){
					// ���݂�����
					vase->SetExist(false);
				}
			}
		}
		else{
			// �j���̏���
			vase->BreacVase();
			// ���͈͈̔ȉ��Ȃ�G���̈ʒu�֗U������
			if (vase->GetBoundingSphere().Radius < 15.0){
				// �U������ʒu��ݒ肷��
				m_enemyManager->SetVacePos(vase->GetPosition());
			}
		}
	}

	// �v���C���[���G�Ƀq�b�g�������ǂ������m�F
	if (!m_player->GetExist() && !m_isDamageHeart){
		// �n�[�g�̌���
		m_ui->CountHeart();
		// ��x�����Ăяo�����߂Ƀt���O���I�t�ɂ���
		m_isDamageHeart = true;
		// �N�[���_�E���^�C�}�[�����Z�b�g
		m_downheartCoolTimer = 0.0f;
	}

	// �N�[���_�E���^�C�}�[���X�V
	if (m_isDamageHeart){
		//���Ԃ��v������
		m_downheartCoolTimer += elapsedTime;
		// 3�b��ɍĂьĂяo����悤�Ƀt���O���I���ɂ���
		if (m_downheartCoolTimer >= 3.0f){
			m_isDamageHeart = false;
		}
	}

	// �S�[���ɐG�ꂽ�ꍇ�̓V�[����ύX
	if (mylib::Collision::BoundingCheckCollision(m_player->GetBoundingBox(), m_goal->GetBoundingSphere())){
		// �V�[���̕ύX��������
		m_isChangeScene = true;
		// �t�F�C�h�p�̎��Ԃ�ݒ�
		m_fade->SetTime(0.0f);
	}

	// �n�[�g���S�ċ�ɂȂ����烊�U���g�V�[���ɑJ�ڂ���
	if (m_ui->AreHeartsEmpty() && !m_isEmptyHart){
		RequestResultScene();
	}
	// ���Ԑ����ɂȂ�����V�[���̕ύX
	if (m_ui->GetNumber()->GetTime() <= 0.0f && !m_isEmptyHart){
		RequestResultScene();
	}

	//�G���S�ł�����N���A
	if (m_enemyManager->GetEmpty() && !m_isChangeScene){
		// �V�[���̕ύX��������
		m_isChangeScene = true;
		// �t�F�C�h�p�̎��Ԃ�ݒ�
		m_fade->SetTime(0.0f);
	}
}


//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void PlayScene::Render()
{
	using namespace DirectX;

	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states =  m_commonResources->GetCommonStates();

	// �����̏���`�悷��
	for (auto& floor : m_floors){
		floor->Render(m_view, m_projection);
	}

	// �ǂ̕`��
	for (const auto& wall : m_walls){	
		// �\������Ă���Ȃ�`��
		if (!wall->IsVisible())wall->Render(context, m_view, m_projection);
	}

	// ���b�V����`�悷��
	for (auto& collisionMesh : m_collisionMesh){
		collisionMesh->Draw(context, states, m_view, m_projection);
	}

	// ��̕`��
	for (auto& vase : m_vase){
		// �₪����`��
		if (vase->GetExist()) vase->Render(context, m_view, m_projection);
	}

	//�G�̕`��
	m_enemyManager->Render(context, m_view, m_projection);

	// �v���C���[��`�悷��
	m_player->Render(m_view, m_projection);

	//�X�J�C���f���̕`��
	RenderSkyModel();

	//�S�[���̕`��
	m_goal->Render(context, m_view, m_projection);

	//UI�̕`��
	m_ui->Render(m_elapsedTime);

	//�t�F�C�h�̕\��
	if (m_fade->GetTime() < 3.25f){
		// ���Ԍo��
		m_fade->Timer(m_elapsedTime);
		//�@�t�F�C�h�̕`��
		m_fade->Render(m_ui.get());
	}


}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void PlayScene::Finalize()
{
	// �S�Ẵ��\�[�X�̉�����s��
	m_floors.clear();
	m_walls.clear();
	m_player.reset();
	m_ui.reset();
	m_goal.reset();
	m_enemyManager.reset();
	Sound::GetInstance().Finalize();

}

//---------------------------------------------------------
// �N���X�̐���
//---------------------------------------------------------
void PlayScene::CreateClass()
{
	auto deviceResources = m_commonResources->GetDeviceResources();

	// �����쐬����
	auto floor = std::make_unique<Floor>();
	floor->Initialize(m_commonResources);
	m_floors.push_back(std::move(floor));

	//�t�F�C�h�𐶐�
	m_fade = std::make_unique<Fade>();
	m_fade->Create(deviceResources);
	// ���Ԃ̃Z�b�g�i�t�F�C�h�A�E�g�p�j
	m_fade->SetTime(1.0f);

	//�ǂ̐���
	m_walls = WallFactory::CreateWalls(m_commonResources, m_typemap, m_map);
	
	//�␶��
	m_collisionMesh = CollisionMeshFactory::CreateCollisionMeshs(m_commonResources,m_collisionMeshTypeMap,m_collisionMeshPosMap,m_collisionMeshRotationMap);

	// TPS�J�������쐬
	m_camera = std::make_unique<Camera>();
	m_view = m_camera->GetViewMatrix();

	// �v���C���[�𐶐�����
	m_player = std::make_unique<PlayerState>(m_camera.get(), m_walls, m_playerPos, m_floors);
	m_player->Initialize(m_commonResources);

	// �G�𐶐�����
	m_enemyManager = std::make_unique<EnemyManager>(m_walls, m_player.get(), m_enemyMap, m_patrolPath, m_straighteningEnemyMap,m_collisionMesh);
	m_enemyManager->Initialize(m_commonResources);

	//UI�̐���
	m_ui = std::make_unique<UI>();
	m_ui->Initialize(m_commonResources);

	// ��̔z�u
	for (int i = 0; i < m_vasePosMap.size(); ++i){
		m_vase.push_back(std::make_unique<Vase>(m_vasePosMap[i]));
		m_vase[i]->Initialize(m_commonResources);
	}
}

//---------------------------------------------------------
//�W�F�C�\���̓ǂݍ���
//---------------------------------------------------------
void PlayScene::loadMapJSON(const char* filename)
{
	assert(filename);

	//���̃t�@�C�������[�h����
	LoadJson loader(filename);
	//���̃t�@�C�������݂��Ȃ��Ȃ��΂�
	if (!loader.IsLoaded()) return;
	
	// �I�u�W�F�N�g���̎擾
	std::vector<ObjectData> walls = loader.GetObjects("walls");
	std::vector<ObjectData> CollisionMesh = loader.GetObjects("CollisionMesh");
	std::vector<ObjectData> enemyObjectects = loader.GetObjects("EnemyPos");
	std::vector<ObjectData> enemyPath = loader.GetObjects("EnemyPath");
	std::vector<ObjectData> straighteningEnemyObjects = loader.GetObjects("StraighteningEnemyPos");
	std::vector<ObjectData> playerPos = loader.GetObjects("PlayerPos");
	std::vector<ObjectData> goalPos = loader.GetObjects("GoalPos");
	std::vector<ObjectData> vasePos = loader.GetObjects("VasePos");

	// �ǂ̔z��
	for (const auto& object : walls) {
		m_typemap.push_back(object.type);
		m_map.push_back(object.position);
	}
	// ��̔z��Ɛ���
	for (const auto& object : CollisionMesh){
		m_collisionMeshPosMap.push_back(object.position);
		m_collisionMeshRotationMap.push_back(object.rotation);
		m_collisionMeshTypeMap.push_back(object.type);
	}
	// �G�̔z��
	for (const auto& object : enemyObjectects) {
		m_enemyMap.push_back(object.position);
	}
	// �G�̃p�X�̔z��
	for (const auto& object : enemyPath) {
		m_patrolPath.push_back(object.position);
	}
	// �ːi�G�̔z��
	for (const auto& object : straighteningEnemyObjects) {
		m_straighteningEnemyMap.push_back(object.position);
	}
	// �v���C���[�̈ʒu
	for (const auto& object : playerPos) {
		m_playerPos = object.position;
	}
	// �S�[��
	for (const auto& object : goalPos) {
		m_goal = std::make_unique<Goal>(object.position);
		m_goal->Initialize(m_commonResources);
	}
	// ��̔z��
	for (const auto& object : vasePos) {
		m_vasePosMap.push_back(object.position);
	}
	
}

//---------------------------------------------------------
// �J�����̉�]������
//---------------------------------------------------------
void PlayScene::CameraRotate()
{
	// �L�[�{�[�h�X�e�[�g���擾����
	DirectX::Keyboard::State keyboardState = DirectX::Keyboard::Get().GetState();
	// �J������]����
	if (keyboardState.A){
		if (!m_isrotateLeftPressed){
			// A�L�[�������ꂽ�u�Ԃ�����]����
			m_camera->RotateCameraAroundPlayer(DirectX::XMConvertToRadians(90.0f)); 
			// �t���O���I���ɂ���
			m_isrotateLeftPressed = true; 
		}
	}
	else{
		// A�L�[�������ꂽ��t���O���I�t�ɂ���
		m_isrotateLeftPressed = false; 
	}
	if (keyboardState.D){
		if (!m_isrotateRightPressed){
			// D�L�[�������ꂽ�u�Ԃ�����]����
			m_camera->RotateCameraAroundPlayer(-DirectX::XMConvertToRadians(90.0f)); 
			// �t���O���I���ɂ���
			m_isrotateRightPressed = true; 
		}
	}
	else{
		// D�L�[�������ꂽ��t���O���I�t�ɂ���
		m_isrotateRightPressed = false; 
	}
}

//---------------------------------------------------------
// ���̃V�[��ID���擾����
//---------------------------------------------------------
IScene::SceneID PlayScene::GetNextSceneID() const
{
	// �V�[���ύX������ꍇ
	if (m_isChangeScene){
		return IScene::SceneID::RESULT;
	}
	// �n�[�g����Ȃ�
	if (m_isEmptyHart){
		//�t�F�C�h���Ăяo��
		m_fade->Render(m_ui.get());
		//1.5�b�ȏ�o������V�[����ς���
		if (m_fade->GetTime() >= 1.5f){
			return IScene::SceneID::TITLE;
		}	
	}
	// �V�[���ύX���Ȃ��ꍇ
	return IScene::SceneID::NONE;
}

//---------------------------------------------------------
// ��̓����蔻��(�v���C���j
//---------------------------------------------------------
void PlayScene::CheckPlayerCollisionMesh()
{
	using namespace DirectX::SimpleMath;

	//�R���W�������b�V���̓����蔻����̎擾
	auto collision = mylib::Collision::CheckCollisionMesh(m_collisionMesh, m_player->GetPosition(),
		m_player->GetForwardDirection());

	// �Փ˂����������ꍇ�̏���
	if (!collision.hasHit) return;

	// �@���x�N�g���𐳋K��
	collision.normal.Normalize();

	// �v���C���[�̈ʒu
	float playerYPosition = m_player->GetPosition().y;
	// �Փ˓_
	float collisionMeshYPosition = collision.hitPosition.y;
	
	// Y���W���r �v���C���[����̏�̏ꍇ
	if (playerYPosition < collisionMeshYPosition){
		Vector3 pushBackPosition;
		// �����Ԃ�
		if (collisionMeshYPosition - playerYPosition > 0.2f){
			// �����Ԃ��̒l�̌v�Z
			pushBackPosition = collision.hitPosition + collision.normal * PUSHBACKSTRENGTH;
		}
		else{
			// �⓹��o���悤�ɏ������������Ԃ�
			pushBackPosition = collision.hitPosition + collision.normal * SMALLPUSHBACKSTRENGTH;
		}
		//�����Ԃ��̒l��ݒ肷��
		m_player->SetPosition(pushBackPosition);
	}
}

//---------------------------------------------------------
// ��̓����蔻��(�{���j
//---------------------------------------------------------
void PlayScene::CheckBomCollisionMesh()
{
	mylib::Collision::CollisionResult collision;

	//�R���W�������b�V���̓����蔻����̎擾
	for (int i = 0; i < PlayerState::BOMMAX; i++){

		// �R���W�����ƃ{���̓����蔻��
		collision = mylib::Collision::CheckCollisionMesh(m_collisionMesh, m_player->GetBomState(i)->GetPosition(), 
			m_player->GetForwardDirection());

		// �Փ˂��������ĂȂ��Ȃ��΂�
		if (!collision.hasHit) continue;

		// �@���x�N�g���𐳋K��
		collision.normal.Normalize();

		// �Փˈʒu��@�������ɏ����␳�i�߂荞�ݖh�~�j
		m_hitPosition += collision.normal;

		// �v���C���[�̃{���ʒu�ƃ{���̏�Ԃ��X�V
		m_player->GetBomState(i)->SetPosition(m_hitPosition);

		//�@�o�E���h�������ɂ���
		m_player->GetBomState(i)->SetBounce(true);
	}
}

//---------------------------------------------------------
// �X�J�C�h�[���̕`��
//---------------------------------------------------------
void PlayScene::RenderSkyModel()
{
	using namespace DirectX;

	auto states = m_commonResources->GetCommonStates();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	//�X�J�C���f���̃G�t�F�N�g�����X�V����
	m_sky->UpdateEffects([](DirectX::IEffect* effect)
		{
			// �x�[�V�b�N�G�t�F�N�g��ݒ肷��
			BasicEffect* basicEffect = dynamic_cast<BasicEffect*>(effect);
			if (basicEffect)
			{
				// �ʂ̃��C�g�����ׂĖ���������
				basicEffect->SetLightEnabled(0, false);
				basicEffect->SetLightEnabled(1, false);
				basicEffect->SetLightEnabled(2, false);
			}
		}
	);

	//���[���h�s����X�V����
	SimpleMath::Matrix world = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(0, 0, 0.0f)) * DirectX::SimpleMath::Matrix::CreateScale(0.5f);

	// �X�J�C���f����`�悷��
	m_sky->Draw(context, *states, world, m_view, m_projection);
}

//---------------------------------------------------------
// ���U���g�`�F���W�p�̕ϐ��̐ݒ�
//---------------------------------------------------------
void PlayScene::RequestResultScene()
{
	// �n�[�g����ɂȂ���
	m_isEmptyHart = true;
	// �t�F�C�h�p�̎��Ԑݒ�
	m_fade->SetTime(0.0f);
}
