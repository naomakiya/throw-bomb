/*
  @file  CollisionMeshFactory.cpp
  @brief 坂工場クラス
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

    // 配列を宣言する
    std::vector<std::unique_ptr<mylib::CollisionMesh>>  CollisionMeshs ;

    // `typemap` と `map` のサイズが一致していることを確認
    if (typemap.size() != mapPos.size() || typemap.size() != mapRotation.size()) {
        throw std::invalid_argument("typemap と mapPos か　typemap と mapRotation　のサイズが一致していません。");
    }

    // 壁の情報を処理する
    for (int i = 0; i < mapPos.size(); ++i) 
    {
        DirectX::SimpleMath::Vector3 position = mapPos[i];  // 壁の位置
        DirectX::SimpleMath::Quaternion rotation = mapRotation[i]; //坂の回転
        int typeValue = typemap[i];

        // 壁を生成する
        auto newCollisionMesh = std::make_unique<mylib::CollisionMesh>(position, rotation);
        // 壁の初期化
        newCollisionMesh->Initialize(device, L"Oblique", typeValue);
        // ベクターに追加
        CollisionMeshs.push_back(std::move(newCollisionMesh));
    }

    // 完成した壁リストを返す
    return CollisionMeshs;
}
