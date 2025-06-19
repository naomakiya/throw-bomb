#pragma once
#ifndef STRAIGHTENINGENEMYSTATE_DEFINED
#define STRAIGHTENINGENEMYSTATE_DEFINED
#include"Interface/IEnemyState.h"


// �O���錾
class CommonResources;
class EnemyScene;
class PlayerState;
class StraighteningEnemyExist;
class StraighteningEnemySearch;
class StraighteningEnemyDash;
//class EnemyStop;
class Shadow;
class Wall;
class CollisionDebugRenderer;

class StraighteningEnemyState : public IEnemyState
{
public:
    // �X�P�[�����擾����
    float GetScale() const { return m_scale; }
    // �ʒu���擾����
    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    // �ʒu��ݒ肷��
    void SetPosition(const DirectX::SimpleMath::Vector3& position){ m_position = position; }
    // �ړI�n�̈ʒu��ݒ�
    void SetPointPosition(DirectX::SimpleMath::Vector3& position) { m_position = position; }
    //�G�̐����擾
    bool GetExist() const { return isExist; }
    //�G�̐����ݒ�
    void SetExist(const bool exist) { isExist = exist; }
    // �o�E���f�B���O�{�b�N�X���擾����
    DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
    // �o�E���f�B���O�{�b�N�X��ݒ肷��
    void SetBoundingBox(const DirectX::BoundingBox box)  { m_boundingBox = box; }
    // �o�E���f�B���O�X�t�B�A���擾����
    DirectX::BoundingSphere GetBoundingSphere() const { return m_boudingSphere; }
    // �o�E���f�B���O�X�t�B�A��ݒ肷��
    void SetBoundingSphere(const DirectX::BoundingSphere sphere) { m_boudingSphere = sphere; }
    //// ���f�����擾����
    DirectX::Model* GetModel()  { return m_enemyModel.get(); }
    // ���݂̌����Ă���������擾����
    DirectX::SimpleMath::Vector3 GetForwardDirection() const { return DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3::Backward, m_rotate); }
    // �v���C���[�X�e�[�g���擾����
    PlayerState* GetPlayer() { return m_player; }

    // �G���擾����
    StraighteningEnemyExist* GetStraighteningEnemyExist() const { return m_exist.get(); }
    // �T����Ԃ��擾����
    StraighteningEnemySearch* GetStraighteningEnemySearch() const { return m_search.get(); }
    //�G�̃_�b�V�����擾����
    StraighteningEnemyDash* GetStraighteningEnemyDash() const {return m_dash.get();}
    ////�G�̎~�܂���擾����
    //EnemyStop* GetEnemyStop() const { return m_stop.get(); }
public:
    // �R���X�g���N�^
    StraighteningEnemyState(const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player);
    // �f�X�g���N�^
    ~StraighteningEnemyState();
    // ����������
    void Initialize(CommonResources* resources, DirectX::SimpleMath::Vector3 pos);
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
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection,
        const DirectX::Model& model) ;
    // �㏈�����s��
    void Finalize() ;
    //�ǂƂ̉����o������
    DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& wallBox);
    // �G���m�̓����蔻����s��
    //void CheckEnemyCollisions(std::vector<std::unique_ptr<StraighteningEnemyState>>& enemies);
    //�G�C�`�s�[�����炷
    void HPDown();
private:
    //�v���C���[
    PlayerState* m_player;
    // ���ʃ��\�[�X
    CommonResources* m_commonResources;
    //�����蔻��̕`��
    std::unique_ptr<CollisionDebugRenderer> m_collisionDebugRenderer;
    //���
    IEnemyState* m_currentState;
    //��
    const std::vector<std::unique_ptr<Wall>>& m_wall;
    // Enemy�̃��f��
    std::unique_ptr<DirectX::Model> m_enemyModel;
    // �o�E���f�B���O�{�b�N�X
    DirectX::BoundingBox m_boundingBox;
    //�o�E�f�B���O�X�t�B�A
    DirectX::BoundingSphere m_boudingSphere;
    //�G�̌o�H�p�X
    std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
    // �e�I�u�W�F�N�g
    std::unique_ptr<Shadow> m_shadow;
    //�����蔻��
    //rcPolyMesh* m_polyMesh;
    // ����
    float m_mass;
    // �p�x
    float m_angle;
    // �X�P�[��
    float m_scale;
    // �ʒu
    DirectX::SimpleMath::Vector3 m_position;
    // �G�̉�]
    DirectX::SimpleMath::Quaternion m_rotate;
    // �e�̈ʒu
    DirectX::SimpleMath::Vector3 m_shadowPosition;
    // ����
    bool isExist;
    //HP
    int m_hart;
    //�������Ă���
    std::unique_ptr<StraighteningEnemyExist> m_exist;
    //�T�����Ă���
    std::unique_ptr<StraighteningEnemySearch> m_search;
    //�����Ă���
    std::unique_ptr<StraighteningEnemyDash> m_dash;
    ////�~�܂��Ă���Ă���
    //std::unique_ptr<EnemyStop> m_stop;
};
#endif // ENEMY_DEFINED