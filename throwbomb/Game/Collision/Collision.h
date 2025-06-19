/*
  @file  Collision.h
  @brief 当たり判定クラス
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
		// コンストラクタ
		Collision() = default;
		//箱と球の判定(バウディング使用しない場合)
		static bool CheckCollision(const DirectX::SimpleMath::Vector3& boxPosition, const float& boxSize,
			const DirectX::SimpleMath::Vector3& spherePosition, float sphereRadius);
		//箱と球の判定（バウディング使用する場合）
		static bool BoundingCheckCollision(const DirectX::BoundingBox& box, const DirectX::BoundingSphere& sphere);
		// 球と球の判定（バウディング使用する場合）
		static bool BoundingSphereCheckCollision(const DirectX::BoundingSphere& sphere1, const DirectX::BoundingSphere& sphere2);
		//箱同士のバウディングボックスの当たり判定
		static bool BoundingCheckBoxCollision(const DirectX::BoundingBox& boxa, const DirectX::BoundingBox& boxb);
		// プレイヤーとテクスチャーの当たり判定
		static bool CheckPlayerTextureCollision(const DirectX::SimpleMath::Vector3& playerPosition, const DirectX::SimpleMath::Vector3& playerSize, const DirectX::SimpleMath::Vector3& texturePosition, const float textureWidth, const float textureDepth);
		// 壁とプレイヤーの当たり判定を行う
		static DirectX::SimpleMath::Vector3 CheckHit(const DirectX::BoundingBox& boxA, const DirectX::BoundingBox& boxB);
		//コリジョンメッシュとの当たり判定
		static CollisionResult CheckCollisionMesh(const std::vector<std::unique_ptr<mylib::CollisionMesh>>& collisionMeshs,
			DirectX::SimpleMath::Vector3 playerPos, DirectX::SimpleMath::Vector3 forwardDirection);
	};
}
#endif		// COLLISION_DEFINED