/*
  @file  Collision.cpp
  @brief �R���W�����N���X
*/
#include "pch.h"
#include "Collision.h"
#include "Libraries/MyLib/CollisionMesh.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

//���Ƌ������蔻��
bool mylib::Collision::CheckCollision(const DirectX::SimpleMath::Vector3& boxPosition, const float& boxSize, const DirectX::SimpleMath::Vector3& spherePosition, float sphereRadius)
{
    // ���̊e�ӂ̒����̔���
    float halfSize = boxSize * 0.5f;

    // �X�t�B�A�̒��S���甠�̒��S�ւ̃x�N�g��
    DirectX::SimpleMath::Vector3 v = boxPosition - spherePosition;

    // ��Βl�Ŋe�������r���āA�_�������̂̓����ɂ��邩�ǂ����𒲂ׂ�
    float dx = std::abs(v.x);
    float dy = std::abs(v.y);
    float dz = std::abs(v.z);

    // ���̔��a�Ɨ����̂̃T�C�Y�̍��v���r���A�Փ˂𔻒肷��
    return (dx <= halfSize + sphereRadius) && (dy <= halfSize + sphereRadius) && (dz <= halfSize + sphereRadius);
}

//�o�E�f�B���O�{�b�N�X�A�X�t�B�A�ł̓����蔻��      
bool  mylib::Collision::BoundingCheckCollision(const DirectX::BoundingBox& box, const DirectX::BoundingSphere& sphere)
{
    // AABB�̍ł��߂��_���v�Z����
    Vector3 closestPoint = sphere.Center;

    // �e���ɂ��āA�X�t�B�A�̒��S��AABB�̊O���ɂ���ꍇ�A���̎���AABB�̒[�_���g�p����
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

    // �ł��߂��_�ƃX�t�B�A�̒��S�Ƃ̋������v�Z����
    Vector3 distanceVec = closestPoint - sphere.Center;
    float distanceSq = distanceVec.LengthSquared();

    // �����̓�悪�X�t�B�A�̔��a�̓��ȉ��ł���΁A�Փ˂��������Ă���
    return distanceSq <= sphere.Radius * sphere.Radius;
}

//���Ƌ������蔻��
bool  mylib::Collision::BoundingSphereCheckCollision(const DirectX::BoundingSphere& sphere1, const DirectX::BoundingSphere& sphere2)
{
    // 2�̋��̒��S�Ԃ̋������v�Z
    DirectX::SimpleMath::Vector3 distanceVec = sphere1.Center - sphere2.Center;
    float distanceSq = distanceVec.LengthSquared();

    // 2�̋��̔��a�̘a���v�Z
    float radiusSum = sphere1.Radius + sphere2.Radius;

    // ���S�Ԃ̋����̓�悪���a�̘a�̓��ȉ��ł���΁A�Փ˂��Ă���
    return distanceSq <= radiusSum * radiusSum;
}

//�����m�̃o�E�f�B���O�{�b�N�X�̓����蔻��
bool  mylib::Collision::BoundingCheckBoxCollision(const DirectX::BoundingBox& boxa, const DirectX::BoundingBox& boxb)
{
    // �o�E���f�B���O�{�b�N�X���m�̏Փ˔�����s��
    return boxa.Intersects(boxb);
}  


//���Ƃ̓����蔻��
bool  mylib::Collision::CheckPlayerTextureCollision(const DirectX::SimpleMath::Vector3& playerPosition,
    const DirectX::SimpleMath::Vector3& playerSize, 
    const DirectX::SimpleMath::Vector3& texturePosition, 
    const float textureWidth, const float textureDepth)
{
    // �e�N�X�`���[�̎l���̍��W���v�Z
    float halfWidth = textureWidth / 2.0f;
    float halfDepth = textureDepth / 2.0f;

    DirectX::SimpleMath::Vector3 textureVertices[4] = {
        DirectX::SimpleMath::Vector3(texturePosition.x - halfWidth, 0.0f, texturePosition.z - halfDepth),   // ����
        DirectX::SimpleMath::Vector3(texturePosition.x + halfWidth, 0.0f, texturePosition.z - halfDepth),   // �E��
        DirectX::SimpleMath::Vector3(texturePosition.x - halfWidth, 0.0f, texturePosition.z + halfDepth),   // ����
        DirectX::SimpleMath::Vector3(texturePosition.x + halfWidth, 0.0f, texturePosition.z + halfDepth)    // �E��
    };

    // �v���C���[�̍��W�ƃT�C�Y����AABB�iAxis-Aligned Bounding Box�j���v�Z
    DirectX::SimpleMath::Vector3 playerMin = playerPosition - playerSize / 2.0f;
    DirectX::SimpleMath::Vector3 playerMax = playerPosition + playerSize / 2.0f;

    // �v���C���[��AABB�ƃe�N�X�`���[�̎l���̍��W�Ƃ̓����蔻����s��
    for (int i = 0; i < 4; ++i) {
        if (textureVertices[i].x >= playerMin.x && textureVertices[i].x <= playerMax.x &&
            textureVertices[i].z >= playerMin.z && textureVertices[i].z <= playerMax.z) {
            return true; // �v���C���[�ƃe�N�X�`���[���d�Ȃ��Ă���
        }
    }

    return false; // �v���C���[�ƃe�N�X�`���[���d�Ȃ��Ă��Ȃ�
}

//�ǂ̉����o��
DirectX::SimpleMath::Vector3  mylib::Collision::CheckHit(const DirectX::BoundingBox& boxA, const DirectX::BoundingBox& boxB)
{
    if (!boxA.Intersects(boxB))
    return DirectX::SimpleMath::Vector3::Zero;

    DirectX::SimpleMath::Vector3 aMin = boxA.Center - boxA.Extents;
    DirectX::SimpleMath::Vector3 aMax = boxA.Center + boxA.Extents;
    DirectX::SimpleMath::Vector3 bMin = boxB.Center - boxB.Extents;
    DirectX::SimpleMath::Vector3 bMax = boxB.Center + boxB.Extents;

    // �߂荞�ݗʁi�����o���K�v�̂���ʁj���v�Z�i�����ŕ������킩��j
    float dx1 = bMax.x - aMin.x; 
    float dx2 = bMin.x - aMax.x;
    float dy1 = bMax.y - aMin.y;
    float dy2 = bMin.y - aMax.y;
    float dz1 = bMax.z - aMin.z;
    float dz2 = bMin.z - aMax.z;

    // ��Βl�������������u�ŏ����̉����o���ʁv
    float dx = (std::abs(dx1) < std::abs(dx2)) ? dx1 : dx2;
    float dy = (std::abs(dy1) < std::abs(dy2)) ? dy1 : dy2;
    float dz = (std::abs(dz1) < std::abs(dz2)) ? dz1 : dz2;

    // �ŏ��̉����o������I��
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
    // �v���C���[�̌��݈ʒu���班����Ƀ��C�̎n�_��ݒ�i�߂荞�ݖh�~�j
    Vector3 rayOrigin = playerPos;
    rayOrigin.y += 0.5f;

    // �v���C���[�̌����Ă�������iXZ���ʂ̂݁j
    Vector3 forwardDir = forwardDirection;
    forwardDir.y = 0.0f;
    forwardDir.Normalize();

    // ���C�̕������X�g�i�������{��Ή��j
    Vector3 downDir = Vector3::Down;
    std::vector<Vector3> rayDirections = { downDir, forwardDir };

    float closestDistance = FLT_MAX;

    // �Փˌ��o
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


