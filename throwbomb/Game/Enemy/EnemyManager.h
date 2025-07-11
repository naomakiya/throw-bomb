/*
    @file	EnemyManager.h
    @brief	�G���܂Ƃ߂�N���X
*/
#pragma once
#ifndef ENEMY_MANAGER_DEFINED
#define ENEMY_MANAGER_DEFINED

// �O���錾
class CommonResources;
class ResourceManager;
class PlayerState;
class Wall;
class IEnemyState;
class StraighteningEnemyState;

namespace mylib
{
    class CollisionMesh;
}

class EnemyManager
{
public: 
    //�G�̐����擾
    bool GetExist() const { return m_exist; }
    //�G�̐����ݒ�
    void SetExist(const bool exist) { m_exist = exist; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    //�S�Ă̓G�̋�̎擾
    bool GetEmpty() const { return m_empty; }
    //�j�󂳂ꂽ��̏ꏊ�̎擾
    void SetVacePos(DirectX::SimpleMath::Vector3 vasePos);
    // ���v�̓G�̐�
    int GetSumEnemy() { return m_sumEnemy; }
    // �|������
    int GetDefeatedEnemies() { return m_defeatedEnemies; }
    // �G�̐�
    int GetEnemyes() { return m_enemyes; }
    // �ːi�̓G�̐�
    int GetStraighteningEnemys(){ return m_straighteningEnemys; }
    //�@�c��̓G�̐�
    int GetRemainingEnemies() { return m_remainingEnemies; }
 public:
    // �R���X�g���N�^
    EnemyManager(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, std::vector<DirectX::SimpleMath::Vector3> enemyMap, const std::vector<DirectX::SimpleMath::Vector3> patrolPath, std::vector<DirectX::SimpleMath::Vector3> straighteningEnemyMap, std::vector<std::unique_ptr<mylib::CollisionMesh>>& collisionMeshes);
    // �f�X�g���N�^
    ~EnemyManager();
    // ����������
    void Initialize(CommonResources* resources);
    // �X�V����
    void Update(const float& elapsedTime);
    // �`�悷��
    void Render(ID3D11DeviceContext* context,
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection);
    // ��n�����s��
    void Finalize();
private:
    // �G�̍X�V
    void EnemyUpdate(const float& elapsedTime);
    // �ːi�G�̍X�V
    void StraighteningEnemyUpdate(const float& elapsedTime);
    // ��̓����蔻��(�G�j
    void CollisionMeshDeterminationEnemy(const std::unique_ptr<IEnemyState>& enemy);
private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // �v���C���[
    PlayerState* m_player;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // �G
    std::vector<std::unique_ptr<IEnemyState>> m_enemy;
    // �ːi����G
    std::vector<std::unique_ptr<IEnemyState>> m_straighteningEnemy;
    // Enemy�̃��f��
    std::unique_ptr<DirectX::Model> m_enemydashuModel;
    // Enemy�̃��f��
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
    // �G�̈ʒu���
    std::vector<DirectX::SimpleMath::Vector3> m_enemyMap;
    // �ːi�G�̈ʒu���
    std::vector<DirectX::SimpleMath::Vector3> m_straighteningEnemyMap;
    //�T���o�H
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
    // �R���W�������b�V���N���X
    std::vector<std::unique_ptr<mylib::CollisionMesh>>& m_collisionMeshes;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �X�P�[��
    float m_scale;
    // �G�̐�
    int m_enemyes;
    // �ːi����G�̐�
    int m_straighteningEnemys;
    // �G�̍��v
    int m_sumEnemy;
    // �|�����G�̐�
    int m_defeatedEnemies;    
    // �c��̓G�̐�
    int m_remainingEnemies;
    // ����
    bool m_exist;
    // �q�b�g�t���O
    bool m_isHit;
    //�S�Ă̓G�����Ȃ��Ȃ���
    bool m_empty;
    // �S�Ẵp�g���[���̓G�����邩
    bool m_isAllEnemiesDefeated;
    // �S�Ă̓ːi����G�����邩
    bool m_isAllStraighteningEnemiesDefeated;
    //��|�W�V�������擾����ϐ�
    DirectX::SimpleMath::Vector3 m_vacePos;
};
#endif // ENEMY_MANAGER_DEFINED
