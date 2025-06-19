/*
  @file PlayScene.h
  @brief �v���C�V�[���N���X
*/
#pragma once
#include"Interface/IScene.h"

#include"nlohmann/json.hpp"
using json = nlohmann::json;

// �O���錾
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
	// �R���X�g���N�g
	PlayScene(SceneManager* sceneManager);
	// �f�X�g���N�^
	~PlayScene() override;
	// ������
	void Initialize(CommonResources* resources) override;
	// �X�V
	void Update(float elapsedTime)override;
	// �`��
	void Render() override;
	// ����
	void Finalize() override;
	// ���̃V�[���̊l��
	SceneID GetNextSceneID() const;
private:
	// �N���X�̐���
	void ClassCreate();
	// �W�F�C�\���̓ǂݍ���
	void loadMapJSON(const char* filename);
	//�J��������
	void CameraRotate();
	//��̓����蔻��(�v���C���j
	void CollisionMeshDeterminationPlayer();
	//��̓����蔻��(�{���j
	void CollisionMeshDeterminationBom();
	//�@�X�J�C���f���̕`��
	void SkyModelRender();
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;
	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;
	// �r���[�s��i�Œ�J�����p�j
	DirectX::SimpleMath::Matrix m_view;
	// TPS�J����
	std::unique_ptr<Camera> m_camera;
	// �v���C���[
	std::unique_ptr<PlayerState> m_player;
	//�G�̊Ǘ�
	std::unique_ptr<EnemyManager> m_enemyManager;
	// ��
	std::vector<std::shared_ptr<Floor>> m_floors;
	// ��
	std::vector<std::unique_ptr<Wall>> m_walls;
	// UI
	std::unique_ptr<UI> m_ui;
	// �S�[��
	std::unique_ptr<Goal> m_goal;
	// 
	SceneManager* m_sceneManager;
	// �X�J�C���f��
	std::unique_ptr<DirectX::Model> m_sky;
	// �ǃ��f��
	std::unique_ptr<DirectX::Model> m_wallModel;
	// �G���f��
	std::unique_ptr<DirectX::Model> m_enemyModel;
	// ���̓O���t�f�[�^
	std::vector<std::string> m_graphData;
	// �}�b�v�f�[�^
	std::vector<DirectX::SimpleMath::Vector3> m_map;
	// �}�b�v�f�[�^
	std::vector<int> m_typemap;
	// �}�b�v�f�[�^
	std::vector<DirectX::SimpleMath::Vector3> m_enemyMap;
	// �}�b�v�f�[�^
	std::vector<DirectX::SimpleMath::Vector3> m_straighteningEnemyMap;
	// ��̈ʒu�}�b�v�f�[�^
	std::vector<DirectX::SimpleMath::Vector3> m_collisionMeshPosMap;
	// ��ނ̈ʒu�}�b�v�f�[�^
	std::vector<int> m_collisionMeshTypeMap;
	// ��]�̈ʒu�}�b�v�f�[�^
	std::vector<DirectX::SimpleMath::Quaternion> m_collisionMeshRotationMap;
	// �G�̌o�H���̕ۑ�
	std::vector<DirectX::SimpleMath::Vector3> m_patrolPath;
	// ��̃}�b�v�f�[�^
	std::vector<DirectX::SimpleMath::Vector3> m_vasePosMap;
	// �R���W�������b�V���N���X
	std::vector<std::unique_ptr<mylib::CollisionMesh>> m_collisionMesh;
	// ��
	std::vector<std::unique_ptr<Vase>> m_vase;
	// �t�F�C�h
	std::unique_ptr<Fade> m_fade;
	// �j��\�ǃ��f��
	std::unique_ptr<DirectX::Model> m_Wallcrack;
private:
	//�o�ߎ���
	float m_elapsedTime;
	// �̗͌��������̎��ɑ̗͌���܂ł̃N�[���^�C��
	float m_heartCooldownTimer = 0.0f;
	//BGM�̉���
	float m_bgmVolume;
	// SE�̉���
	float m_seVolume;
	//�v���C���[�̏ꏊ�̕ۑ�
	DirectX::SimpleMath::Vector3 m_playerPos;
	// �|���S����Ray���Փ˂������W
	DirectX::SimpleMath::Vector3 m_hitPosition;
	// �Փ˂����|���S���̔ԍ�
	int m_hitPolygonNo;
	// �Փ˂����|���S���̖@���̌X��
	DirectX::SimpleMath::Matrix m_rotateNormal;
	// �V�[���`�F���W�t���O
	bool m_isChangeScene;
	// �V�[���`�F���W�t���O(HP�j
	bool m_iseEmptyHart;
	//����������Ă�����
	bool isSpaceHeld = false;
	bool on = false;
	bool m_canCountHeart = true;
	bool m_canCreateBomb = false;
	bool m_holdBom = false;
	bool m_se = false;
	bool rotateLeftPressed = false;
	bool rotateRightPressed = false;

};