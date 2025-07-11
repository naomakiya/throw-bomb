/*
    @file	EnemyManager.cpp
    @brief	�G���܂Ƃ߂�N���X
*/
#include"pch.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/Enemy/EnemyManager.h"
#include "Game/Factory/EnemyFactory.h"
#include "Game/Factory/StraighteningEnemyFactory.h"
#include "Game/Player/PlayerState.h"
#include "Game/Bom/BomState.h"
#include "Game/Collision/Collision.h"
#include "Game/ResourceManager/ResourceManager.h"
#include "Libraries/MyLib/CollisionMesh.h"

void EnemyManager::SetVacePos(DirectX::SimpleMath::Vector3 vasePos)
{
    m_vacePos = vasePos;
    for (auto& enemy : m_enemy)
    {
        float distance = (enemy->GetPosition() - vasePos).Length();
        // 15�ȓ��ɂ���G�ɂ����K�p
        if (distance <= 15.0f) 
        {
            enemy->SetPointPosition(vasePos);
        }
    }
}

//---------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------
EnemyManager::EnemyManager(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, std::vector<DirectX::SimpleMath::Vector3> enemyMap,const std::vector<DirectX::SimpleMath::Vector3> patrolPath, std::vector<DirectX::SimpleMath::Vector3> straighteningEnemyMap,
    std::vector<std::unique_ptr<mylib::CollisionMesh>>& collisionMeshes)
    :
    m_position{},
    m_commonResources{},
    m_enemy{},
    m_straighteningEnemy{},
    m_boundingBox{},
    m_isHit{},
    m_wall{wall},
    m_player{player},
    m_enemyMap{ enemyMap },
    m_patrolPath{ patrolPath },
    m_exist{true},
    m_scale{},
    m_straighteningEnemyMap{ straighteningEnemyMap },
    m_enemydashuModel{},
    m_empty{},
    m_collisionMeshes{ collisionMeshes },
    m_isAllStraighteningEnemiesDefeated{ false },
    m_isAllEnemiesDefeated{ false },
    m_defeatedEnemies{0}
{

}

//---------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------
EnemyManager::~EnemyManager()
{
}

//---------------------------------------------------------
// ����������
//---------------------------------------------------------
void EnemyManager::Initialize(CommonResources* resources)
{
    assert(resources);
   
    m_commonResources = resources;

    auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();

    //�G�̃��f���̓ǂݍ��ݏ���
    std::unique_ptr<DirectX::EffectFactory> fx = std::make_unique<DirectX::EffectFactory>(device);
    fx->SetDirectory(L"Resources/Models/Enemy");

    //�G�̓ǂݎ��
    m_enemyModel = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("Enemy").c_str(), *fx);
    m_enemydashuModel = DirectX::Model::CreateFromCMO(device, ResourceManager::GetModelPath("DashuEnemy").c_str(), *fx);
    // �G�𐶐�����
    m_enemy = EnemyFactory::CreateEnemy(m_wall, m_player, m_commonResources, m_enemyMap, m_patrolPath);
    //�ːi����G�𐶐�
    m_straighteningEnemy = StraighteningEnemyFactory::CreateEnemy(m_wall, m_player, m_commonResources, m_straighteningEnemyMap, m_patrolPath);


    m_enemyes = static_cast<int>(m_enemy.size());
    m_straighteningEnemys = static_cast<int>(m_straighteningEnemy.size());
    m_sumEnemy = m_enemyes + m_straighteningEnemys;
}

//---------------------------------------------------------
// �X�V����
//---------------------------------------------------------
void EnemyManager::Update(const float& elapsedTime)
{
    m_defeatedEnemies = 0;
    //�G
    EnemyUpdate(elapsedTime);
   //�ːi����G
    StraighteningEnemyUpdate(elapsedTime);
    //�c��̓G�̐�
    m_remainingEnemies = m_enemyes + m_straighteningEnemys;
   
    //�S�Ă̓G�����Ȃ��Ȃ�G������������Ԃ��I���ɂ���
    if (m_isAllEnemiesDefeated && m_isAllStraighteningEnemiesDefeated) {
        m_empty = true;
    }
}

//---------------------------------------------------------
// �`�悷��
//---------------------------------------------------------
void EnemyManager::Render(ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection)
{
    //�ǐՃG�l�~�[
    for (auto& enemy : m_enemy)
    {
        //�G�������Ă��邩�����Ă��Ȃ��m�F
        if (enemy->GetExist())enemy->Render(context, view, projection, *m_enemyModel);
    }
    //�@�ːi����G
    for (auto& straighteningEnemy : m_straighteningEnemy)
    {
        //�����Ă���Ȃ�
        if (straighteningEnemy->GetExist()) straighteningEnemy->Render(context, view, projection, *m_enemydashuModel);
    }
}

//---------------------------------------------------------
// ��n������
//---------------------------------------------------------
void EnemyManager::Finalize()
{
    m_enemy.clear();
    m_straighteningEnemy.clear();
}

//---------------------------------------------------------
// �G�̍X�V����
//---------------------------------------------------------
void EnemyManager::EnemyUpdate(const float& elapsedTime)
{
    m_isAllEnemiesDefeated = true; // ���ׂĂ̓G���|���ꂽ�����肷��t���O
    m_enemyes = static_cast<int>(m_enemy.size());
    //�X�V
    for (auto& enemy : m_enemy){
        //�����Ă���Ȃ�
        if (enemy->GetExist()) {
            enemy->Update(elapsedTime);
            m_isAllEnemiesDefeated = false; // ��̂ł��������Ă���Ȃ� false �ɂ���
           
        }
        else
        {
            m_enemyes--;
            m_defeatedEnemies++;
        }
    }

    //�����蔻��
    for (auto& enemy : m_enemy)
    {
        CollisionMeshDeterminationEnemy(enemy);
  
        // �G�������Ă��Ȃ��Ȃ�X�L�b�v
        if (!enemy->GetExist()) continue;

        for (int i = 0; i < PlayerState::BOMMAX; i++){
            //�{�����\������Ă���Ȃ�
            if (m_player->GetBomState(i)->GetExist()){
                //�G�ƃ{�������������Ȃ�
                if (mylib::Collision::BoundingCheckCollision(enemy->GetBoundingBox(), m_player->GetBomState(i)->GetBoundingSphere())) {   // ������ԂȂ�
                    if (m_player->GetBomState(i)->GetBomPresent() == BomState::EXPLOSION) enemy->HPDown();
                }
            }
        }

        // �v���C�������G�łȂ��Ȃ�
        if (!m_player->GetHitEnemy()){
            //�v���C���[�ƓG�̓����蔻��
            if (mylib::Collision::BoundingCheckBoxCollision(m_player->GetBoundingBox(), enemy->GetBoundingBox())) m_player->SetHitEnemy(true);    
        }
    }
  
}

//---------------------------------------------------------
// �ːi�G�̍X�V����
//---------------------------------------------------------
void EnemyManager::StraighteningEnemyUpdate(const float& elapsedTime)
{
    m_isAllStraighteningEnemiesDefeated = true;
    m_straighteningEnemys = static_cast<int>(m_straighteningEnemy.size());
    // �ːi�G�̍X�V
    for (auto& straighteningEnemy : m_straighteningEnemy){
        //�����Ă���Ȃ�
        if (straighteningEnemy->GetExist()) {
            straighteningEnemy->Update(elapsedTime);
            m_isAllStraighteningEnemiesDefeated = false;
           
        }
        else
        {
            m_straighteningEnemys--;
            m_defeatedEnemies++;
        }
    }

    //�G�̓����蔻��
    for (auto& enemy : m_straighteningEnemy){
        // �G�������Ă��Ȃ��Ȃ�X�L�b�v
        if (!enemy->GetExist()) continue;

        CollisionMeshDeterminationEnemy(move(enemy));

        for (int i = 0; i < PlayerState::BOMMAX; i++) {
            //�{�����`�悳��Ă���Ȃ�
            if (m_player->GetBomState(i)->GetExist()){   
                //�G�ƃ{�������������Ȃ�
                if (mylib::Collision::BoundingCheckCollision(enemy->GetBoundingBox(), m_player->GetBomState(i)->GetBoundingSphere())){  
                    // ������ԂȂ�
                    if (m_player->GetBomState(i)->GetBomPresent() == BomState::EXPLOSION) enemy->HPDown();
                }
            }
        }

        //�G�ƃv���C���[�������Ă���Ȃ�
        if (!m_player->GetHitEnemy()) {
            //�v���C���[�ƓG�̓����蔻��
            if (mylib::Collision::BoundingCheckBoxCollision(m_player->GetBoundingBox(), enemy->GetBoundingBox())) {
                m_player->SetHitEnemy(true);
            }
        }
    }
}

//---------------------------------------------------------
// ��̓����蔻��(�G�j
//---------------------------------------------------------
void EnemyManager::CollisionMeshDeterminationEnemy(const std::unique_ptr<IEnemyState>& enemy)
{
    using namespace DirectX::SimpleMath;

    //�R���W�������b�V���̓����蔻����̎擾
    mylib::Collision::CollisionResult collision = mylib::Collision::CheckCollisionMesh(m_collisionMeshes, enemy->GetPosition(), m_player->GetForwardDirection());

    // �Փ˂����������ꍇ�̏���
    if (!collision.hasHit) return;

    collision.normal.Normalize();

    // �G�̈ʒu�ƏՓ˓_��Y���W���r
    float enemyYPosition = enemy->GetPosition().y;
    float collisionMeshYPosition = collision.hitPosition.y;

    // �G���R���W�������b�V������ɂ��Ȃ��ꍇ
    if (enemyYPosition <= collisionMeshYPosition){
        // ���������Ԃ��ꍇ
        if (collisionMeshYPosition - enemyYPosition > 0.25f) {
            float pushBackStrength = 0.5f;
            Vector3 pushBackPosition = collision.hitPosition + collision.normal * pushBackStrength;

            enemy->SetPosition(pushBackPosition);
        }
        else{
            // �⓹��o���悤�ɏ������������Ԃ�
            float smallPushBackStrength = 0.01f;
            Vector3 pushBackPosition = collision.hitPosition + collision.normal * smallPushBackStrength;

            enemy->SetPosition(pushBackPosition);

        }
    }
}