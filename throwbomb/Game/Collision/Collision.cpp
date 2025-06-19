/*
  @file  Collision.cpp
  @brief コリジョンクラス
*/
#include "pch.h"
#include "Collision.h"
#include "Libraries/MyLib/CollisionMesh.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

//箱と球当たり判定
bool mylib::Collision::CheckCollision(const DirectX::SimpleMath::Vector3& boxPosition, const float& boxSize, const DirectX::SimpleMath::Vector3& spherePosition, float sphereRadius)
{
    // 箱の各辺の長さの半分
    float halfSize = boxSize * 0.5f;

    // スフィアの中心から箱の中心へのベクトル
    DirectX::SimpleMath::Vector3 v = boxPosition - spherePosition;

    // 絶対値で各成分を比較して、点が立方体の内側にあるかどうかを調べる
    float dx = std::abs(v.x);
    float dy = std::abs(v.y);
    float dz = std::abs(v.z);

    // 球の半径と立方体のサイズの合計を比較し、衝突を判定する
    return (dx <= halfSize + sphereRadius) && (dy <= halfSize + sphereRadius) && (dz <= halfSize + sphereRadius);
}

//バウディングボックス、スフィアでの当たり判定      
bool  mylib::Collision::BoundingCheckCollision(const DirectX::BoundingBox& box, const DirectX::BoundingSphere& sphere)
{
    // AABBの最も近い点を計算する
    Vector3 closestPoint = sphere.Center;

    // 各軸について、スフィアの中心がAABBの外側にある場合、その軸のAABBの端点を使用する
    if (sphere.Center.x < box.Center.x - box.Extents.x)
        closestPoint.x = box.Center.x - box.Extents.x;
    else if (sphere.Center.x > box.Center.x + box.Extents.x)
        closestPoint.x = box.Center.x + box.Extents.x;

    if (sphere.Center.y < box.Center.y - box.Extents.y)
        closestPoint.y = box.Center.y - box.Extents.y;
    else if (sphere.Center.y > box.Center.y + box.Extents.y)
        closestPoint.y = box.Center.y + box.Extents.y;

    if (sphere.Center.z < box.Center.z - box.Extents.z)
        closestPoint.z = box.Center.z - box.Extents.z;
    else if (sphere.Center.z > box.Center.z + box.Extents.z)
        closestPoint.z = box.Center.z + box.Extents.z;

    // 最も近い点とスフィアの中心との距離を計算する
    Vector3 distanceVec = closestPoint - sphere.Center;
    float distanceSq = distanceVec.LengthSquared();

    // 距離の二乗がスフィアの半径の二乗以下であれば、衝突が発生している
    return distanceSq <= sphere.Radius * sphere.Radius;
}

//球と球当たり判定
bool  mylib::Collision::BoundingSphereCheckCollision(const DirectX::BoundingSphere& sphere1, const DirectX::BoundingSphere& sphere2)
{
    // 2つの球の中心間の距離を計算
    DirectX::SimpleMath::Vector3 distanceVec = sphere1.Center - sphere2.Center;
    float distanceSq = distanceVec.LengthSquared();

    // 2つの球の半径の和を計算
    float radiusSum = sphere1.Radius + sphere2.Radius;

    // 中心間の距離の二乗が半径の和の二乗以下であれば、衝突している
    return distanceSq <= radiusSum * radiusSum;
}

//箱同士のバウディングボックスの当たり判定
bool  mylib::Collision::BoundingCheckBoxCollision(const DirectX::BoundingBox& boxa, const DirectX::BoundingBox& boxb)
{
    // バウンディングボックス同士の衝突判定を行う
    return boxa.Intersects(boxb);
}  


//床との当たり判定
bool  mylib::Collision::CheckPlayerTextureCollision(const DirectX::SimpleMath::Vector3& playerPosition,
    const DirectX::SimpleMath::Vector3& playerSize, 
    const DirectX::SimpleMath::Vector3& texturePosition, 
    const float textureWidth, const float textureDepth)
{
    // テクスチャーの四隅の座標を計算
    float halfWidth = textureWidth / 2.0f;
    float halfDepth = textureDepth / 2.0f;

    DirectX::SimpleMath::Vector3 textureVertices[4] = {
        DirectX::SimpleMath::Vector3(texturePosition.x - halfWidth, 0.0f, texturePosition.z - halfDepth),   // 左上
        DirectX::SimpleMath::Vector3(texturePosition.x + halfWidth, 0.0f, texturePosition.z - halfDepth),   // 右上
        DirectX::SimpleMath::Vector3(texturePosition.x - halfWidth, 0.0f, texturePosition.z + halfDepth),   // 左下
        DirectX::SimpleMath::Vector3(texturePosition.x + halfWidth, 0.0f, texturePosition.z + halfDepth)    // 右下
    };

    // プレイヤーの座標とサイズからAABB（Axis-Aligned Bounding Box）を計算
    DirectX::SimpleMath::Vector3 playerMin = playerPosition - playerSize / 2.0f;
    DirectX::SimpleMath::Vector3 playerMax = playerPosition + playerSize / 2.0f;

    // プレイヤーのAABBとテクスチャーの四隅の座標との当たり判定を行う
    for (int i = 0; i < 4; ++i) {
        if (textureVertices[i].x >= playerMin.x && textureVertices[i].x <= playerMax.x &&
            textureVertices[i].z >= playerMin.z && textureVertices[i].z <= playerMax.z) {
            return true; // プレイヤーとテクスチャーが重なっている
        }
    }

    return false; // プレイヤーとテクスチャーが重なっていない
}

//壁の押し出し
DirectX::SimpleMath::Vector3  mylib::Collision::CheckHit(const DirectX::BoundingBox& boxA, const DirectX::BoundingBox& boxB)
{
    if (!boxA.Intersects(boxB))
    return DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 aMin = boxA.Center - boxA.Extents;
    DirectX::SimpleMath::Vector3 aMax = boxA.Center + boxA.Extents;
    DirectX::SimpleMath::Vector3 bMin = boxB.Center - boxB.Extents;
    DirectX::SimpleMath::Vector3 bMax = boxB.Center + boxB.Extents;

    // めり込み量（押し出す必要のある量）を計算（正負で方向もわかる）
    float dx1 = bMax.x - aMin.x; 
    float dx2 = bMin.x - aMax.x;
    float dy1 = bMax.y - aMin.y;
    float dy2 = bMin.y - aMax.y;
    float dz1 = bMax.z - aMin.z;
    float dz2 = bMin.z - aMax.z;

    // 絶対値が小さい方が「最小限の押し出し量」
    float dx = (std::abs(dx1) < std::abs(dx2)) ? dx1 : dx2;
    float dy = (std::abs(dy1) < std::abs(dy2)) ? dy1 : dy2;
    float dz = (std::abs(dz1) < std::abs(dz2)) ? dz1 : dz2;

    // 最小の押し出し軸を選ぶ
    DirectX::SimpleMath::Vector3 pushBackVec = DirectX::SimpleMath::Vector3::Zero;

    if (std::abs(dx) <= std::abs(dy) && std::abs(dx) <= std::abs(dz))
        pushBackVec.x = dx;
    else if (std::abs(dy) <= std::abs(dx) && std::abs(dy) <= std::abs(dz))
        pushBackVec.y = dy;
    else
        pushBackVec.z = dz;

    return pushBackVec;
}

mylib::Collision::CollisionResult  mylib::Collision::CheckCollisionMesh(const std::vector<std::unique_ptr<mylib::CollisionMesh>>& collisionMeshs, DirectX::SimpleMath::Vector3 playerPos, DirectX::SimpleMath::Vector3 forwardDirection)
{
    CollisionResult result;

    result.hasHit = false;
    // プレイヤーの現在位置から少し上にレイの始点を設定（めり込み防止）
    Vector3 rayOrigin = playerPos;
    rayOrigin.y += 0.5f;

    // プレイヤーの向いている方向（XZ平面のみ）
    Vector3 forwardDir = forwardDirection;
    forwardDir.y = 0.0f;
    forwardDir.Normalize();

    // レイの方向リスト（下方向＋坂対応）
    Vector3 downDir = Vector3::Down;
    std::vector<Vector3> rayDirections = { downDir, forwardDir };

    float closestDistance = FLT_MAX;

    // 衝突検出
    for (const auto& dir : rayDirections)
    {
        float rayLength = 0.5f;
        Ray ray{ rayOrigin, dir };

        for (auto& collisionMesh : collisionMeshs)
        {
            Vector3 tempHitPosition;
            Vector3 tempNormal;
            int tempPolygonNo;

            if (collisionMesh->IntersectRay(ray, &tempHitPosition, &tempNormal, &tempPolygonNo))
            {
                float distance = (tempHitPosition - rayOrigin).Length();
                if (distance < closestDistance && distance <= rayLength)
                {
                    closestDistance = distance;
                    result.hasHit = true;
                    result.hitPosition = tempHitPosition;
                    result.normal = tempNormal;
                    result.polygonNo = tempPolygonNo;
                }

                if (result.hasHit)
                {
                    return result;
                }
            }
        }
    }

    return result;
}


