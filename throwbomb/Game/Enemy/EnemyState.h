/*
  @file�@EnemyState.h
  @brief �G�̏�ԃN���X
*/
#pragma once
#ifndef ENEMYSTATE_DEFINED
#define ENEMYSTATE_DEFINED
#include"Interface/IEnemyState.h"

// �O���錾
class CommonResources;
class EnemyScene;
class PlayerState;
class EnemyExist;
class EnemyMovement;
class EnemySearch;
class EnemyTracking;
class EnemyStop;
class EnemyPointMovement;
class CollisionDebugRenderer;
class Shadow;
class Wall;


class EnemyState : public IEnemyState
{
public: //interface���ł̍쐬
    // �ʒu���擾����
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector3& position){ m_position = position; }
    // �ړI�n�̈ʒu��ݒ�
    void SetPointPosition(DirectX::SimpleMath::Vector3& position);
    //�ړI�n�̎擾
    DirectX::SimpleMath::Vector3 GetPointPosition() const { return m_pointPosition; }
    //�G�̐����擾
    bool GetExist() const { return isExist; }
    //�G�̐����ݒ�
    void SetExist(const bool exist) { isExist = exist; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
public:
    static constexpr float ENEMYMODELSCALE = 0.006f;
public: //���̃N���X�ł̍쐬
    // �o�E���f�B���O�{�b�N�X��ݒ肷��
    void SetBoundingBox(const DirectX::BoundingBox box)  { m_boundingBox = box; }
    // �o�E���f�B���O�X�t�B�A���擾����
    DirectX::BoundingSphere GetBoundingSphere() const { return m_boudingSphere; }
    // �o�E���f�B���O�X�t�B�A��ݒ肷��
    void SetBoundingSphere(const DirectX::BoundingSphere sphere) { m_boudingSphere = sphere; }
    // �v���C���[�X�e�[�g���擾����
    PlayerState* GetPlayer() const { return m_player; }
    // �G�A�C�h�����O���擾����
    EnemyExist* GetEnemyExist() const { return m_exist.get(); }
    // �G�X�g���C�N�t�H���[�f�B���O���擾����
    EnemyMovement* GetEnemyMovement() const { return m_movement.get(); }
    // �G�̒ǐՃ��[�h���擾����
    EnemyTracking* GetEnemyTracking() const {return m_tracking.get();}
    // �G�̎~�܂���擾����
    EnemyStop* GetEnemyStop() const { return m_stop.get(); }
    // �v���C����T��
    EnemySearch* GetEnemySearch() const { return m_search.get();}

public:
    // �R���X�g���N�^
    EnemyState(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, const std::vector<DirectX::SimpleMath::Vector3>& patrolPath,const DirectX::SimpleMath::Vector3& position);
    // �f�X�g���N�^
    ~EnemyState();
    // ����������
    void Initialize(CommonResources* resources);
    // �V������ԂɑJ�ڂ���
    void ChangeState(IEnemyState* newState);
    // ���O�X�V
    void PreUpdate() ;
    // �X�V����
    void Update(const float& elapsedTime) ;
    // ����X�V
    void PostUpdate() ;
    // �`�悷��
    void Render(ID3D11DeviceContext* context,
        DirectX::CommonStates* states,
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection,
        const DirectX::Model& model) ;
    // �㏈�����s��
    void Finalize() ;
    //�G�C�`�s�[�����炷
    void HPDown();

private:
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    // �v���C���[
    PlayerState* m_player;
    // �f�o�b�N�R���W����
    std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;
    // ���
    IEnemyState* m_currentState;
    // ��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemy�̃��f��
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
    //�o�E�f�B���O�X�t�B�A
    DirectX::BoundingSphere m_boudingSphere;
    // �G�̌o�H�p�X
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
    // �e�I�u�W�F�N�g
    std::unique_ptr<Shadow> m_shadow;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotate;
    // ������
    float m_totalSeconds;
    // �p�x
    float m_angle;
    // �X�P�[��
    float m_scale;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �ړI�̈ʒu
    DirectX::SimpleMath::Vector3 m_pointPosition;
    // ����
    bool isExist;
    // HP
    int m_hart;
    // �������Ă���
    std::unique_ptr<EnemyExist> m_exist;
    // �����Ă���
    std::unique_ptr<EnemyMovement> m_movement;
    // �ړI�n�ֈړ�
    std::unique_ptr<EnemyPointMovement> m_pointMovement;
    // �ǐՂ��Ă���
    std::unique_ptr<EnemyTracking> m_tracking;
    // �~�܂��Ă���Ă���
    std::unique_ptr<EnemyStop> m_stop;
    // �T���Ă��� 
    std::unique_ptr<EnemySearch> m_search;

    // ����o�H
    std::vector<DirectX::SimpleMath::Vector3> patrolPath; 
};
#endif // ENEMY_DEFINED