/*
  @file  CollisionMeshFactory.cpp
  @brief ��H��N���X
*/
#include "pch.h"
#include "Game/Factory/CollisionMeshFactory.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"

std::vector<std::unique_ptr<mylib::CollisionMesh>>CollisionMeshFactory::CreateCollisionMeshs(
    CommonResources* commonResources, 
    std::vector<int> typemap,
    std::vector<DirectX::SimpleMath::Vector3> mapPos,
    std::vector<DirectX::SimpleMath::Quaternion> mapRotation)
{
    auto device = commonResources->GetDeviceResources()->GetD3DDevice();

    // �z���錾����
    std::vector<std::unique_ptr<mylib::CollisionMesh>>  CollisionMeshs ;

    // `typemap` �� `map` �̃T�C�Y����v���Ă��邱�Ƃ��m�F
    if (typemap.size() != mapPos.size() || typemap.size() != mapRotation.size()) {
        throw std::invalid_argument("typemap �� mapPos ���@typemap �� mapRotation�@�̃T�C�Y����v���Ă��܂���B");
    }

    // �ǂ̏�����������
    for (int i = 0; i < mapPos.size(); ++i) 
    {
        DirectX::SimpleMath::Vector3 position = mapPos[i];  // �ǂ̈ʒu
        DirectX::SimpleMath::Quaternion rotation = mapRotation[i]; //��̉�]
        int typeValue = typemap[i];

        // �ǂ𐶐�����
        auto newCollisionMesh = std::make_unique<mylib::CollisionMesh>(position, rotation);
        // �ǂ̏�����
        newCollisionMesh->Initialize(device, L"Oblique", typeValue);
        // �x�N�^�[�ɒǉ�
        CollisionMeshs.push_back(std::move(newCollisionMesh));
    }

    // ���������ǃ��X�g��Ԃ�
    return CollisionMeshs;
}
