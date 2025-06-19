/*
  @file  Collision.h
  @brief �����蔻��N���X
*/
#pragma once
#ifndef  COLLISION_DEFINED
#define  COLLISION_DEFINED

namespace mylib
{
	class CollisionMesh;

	class Collision
	{
	public:
		struct CollisionResult
		{
			bool hasHit = false;
			DirectX::SimpleMath::Vector3 hitPosition;
			DirectX::SimpleMath::Vector3 normal;
			int polygonNo = 0;
		};
	public:
		// �R���X�g���N�^
		Collision() = default;
		//���Ƌ��̔���(�o�E�f�B���O�g�p���Ȃ��ꍇ)
		static bool CheckCollision(const DirectX::SimpleMath::Vector3& boxPosition, const float& boxSize,
			const DirectX::SimpleMath::Vector3& spherePosition, float sphereRadius);
		//���Ƌ��̔���i�o�E�f�B���O�g�p����ꍇ�j
		static bool BoundingCheckCollision(const DirectX::BoundingBox& box, const DirectX::BoundingSphere& sphere);
		// ���Ƌ��̔���i�o�E�f�B���O�g�p����ꍇ�j
		static bool BoundingSphereCheckCollision(const DirectX::BoundingSphere& sphere1, const DirectX::BoundingSphere& sphere2);
		//�����m�̃o�E�f�B���O�{�b�N�X�̓����蔻��
		static bool BoundingCheckBoxCollision(const DirectX::BoundingBox& boxa, const DirectX::BoundingBox& boxb);
		// �v���C���[�ƃe�N�X�`���[�̓����蔻��
		static bool CheckPlayerTextureCollision(const DirectX::SimpleMath::Vector3& playerPosition, const DirectX::SimpleMath::Vector3& playerSize, const DirectX::SimpleMath::Vector3& texturePosition, const float textureWidth, const float textureDepth);
		// �ǂƃv���C���[�̓����蔻����s��
		static DirectX::SimpleMath::Vector3 CheckHit(const DirectX::BoundingBox& boxA, const DirectX::BoundingBox& boxB);
		//�R���W�������b�V���Ƃ̓����蔻��
		static CollisionResult CheckCollisionMesh(const std::vector<std::unique_ptr<mylib::CollisionMesh>>& collisionMeshs,
			DirectX::SimpleMath::Vector3 playerPos, DirectX::SimpleMath::Vector3 forwardDirection);
	};
}
#endif		// COLLISION_DEFINED