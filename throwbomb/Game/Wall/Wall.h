/*
  @file  Wall.h
  @brief �ǃN���X
*/

#pragma once
#ifndef WALL_DEFINED
#define WALL_DEFINED

// �O���錾
class CommonResources;
class CollisionDebugRenderer;

class Wall
{
public:
	//�ǂ̎��
	enum class WallType {
		NONE = 0,			 //	���݂��Ȃ�
		Indestructible = 1,  // �j��s�\�ȕ�
		Destructible = 2     // �j��\�ȕ�
	};

public:
	// �X�P�[�����擾����
	float GetScale() const { return m_scale; }
	// �X�P�[����ݒ肷��
	void SetScale(const float scale) { m_scale = scale; }
	// �ʒu���擾����
	DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
	// �ʒu��ݒ肷��
	void SetPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
	//�����擾
	bool GetExist() const { return m_exist; }
	//�����ݒ�
	void SetExist(const bool exist) { m_exist = exist; }
	// �o�E���f�B���O�{�b�N�X���擾����
	DirectX::BoundingBox GetBoundingBox() const { return m_boundingBox; }
	//�ǂ̎�ނ̊m�F
	WallType GetWallType() const { return m_wallType; }
	// ����Ԃ��擾
	bool IsVisible() const { return m_isVisible; }
	// ����Ԃ�ݒ�
	void SetVisible(bool visible) { m_isVisible = visible; }

public:
	// �R���X�g���N�^
	Wall(WallType wallType, std::shared_ptr<DirectX::Model> model,const DirectX::SimpleMath::Vector3& position);
	//�f�X�g���N�^
	~Wall();
	//����������
	void Initialize(CommonResources* resources);
	//�`�悷��
	void Render(ID3D11DeviceContext* context,
		const DirectX::SimpleMath::Matrix& view,
		const DirectX::SimpleMath::Matrix& projection);
	//�㏈�����s��
	void Finalize();
	//�ǂ̃o�E���f�B���O�{�b�N�X���m�̓����蔻��
	DirectX::SimpleMath::Vector3 CheckHitAndResolve(const DirectX::BoundingBox& a, const DirectX::BoundingBox& b);

private:
      // ���ʃ��\�[�X
      CommonResources* m_commonResources;  
      // �f�o�b�O�`��
      std::unique_ptr<CollisionDebugRenderer>  m_collisionDebugRenderer;  
	  // �[�x�X�e���V���X�e�[�g
	  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	  // �e�N�X�`�����\�[�X�r���[
	  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;  
	  // �ǂ̎��
      WallType m_wallType; 
	  // �ǃ��f��
	  std::shared_ptr<DirectX::Model> m_wallModel;
	  // �o�E���f�B���O�{�b�N�X�i�����蔻��p�j
      DirectX::BoundingBox m_boundingBox;  
	  // �ǂ̈ʒu
	  DirectX::SimpleMath::Vector3 m_position;
	  // �X�P�[���{��
	  float m_scale;
	  //���f���X�P�[��
	  float m_modelScale;
	   // �����t���O
      bool m_exist; 
	  // ���t���O
      bool m_isVisible; 
	  // �Փ˂�臒l�i�߂荞�ݗʂ����ꖢ���Ȃ疳���j
	  static float THRESHOLD;
};
#endif		// PLAYER_DEFINED