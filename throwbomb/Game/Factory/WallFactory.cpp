/*
  @file  WallFactory.cpp
  @brief 壁工場クラス
*/
#include "pch.h"
#include "Game/Factory/WallFactory.h"
#include "Framework/CommonResources.h"
#include "Framework/DeviceResources.h"
#include "Game/ResourceManager/ResourceManager.h"

std::vector<std::unique_ptr<Wall>> WallFactory::CreateWalls(
    CommonResources* commonResources, 
    std::vector<int> typemap,
    std::vector<DirectX::SimpleMath::Vector3> map)
{
    using namespace DirectX;


    auto device = commonResources->GetDeviceResources()->GetD3DDevice();
    // 配列を宣言する
    std::vector<std::unique_ptr<Wall>> walls ;

    // `typemap` と `map` のサイズが一致していることを確認
    if (typemap.size() != map.size()) {
        throw std::invalid_argument("typemap と map のサイズが一致していません。");
    }

    //読み込み準備
    std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
    fx->SetDirectory(L"Resources/Models");
    //壁モデルの読み取り
    std::shared_ptr<Model> wallModel = Model::CreateFromCMO(device, ResourceManager::GetModelPath("Wall").c_str(), *fx);
    if (wallModel == nullptr)
    {
        assert(wallModel);
    }
    // 下記が無いと白くなりすぎる
    wallModel->UpdateEffects([&](IEffect* effect) {
        BasicEffect* basic = dynamic_cast<BasicEffect*>(effect);
        if (basic != nullptr) {
            basic->SetDiffuseColor(Colors::Black);

        }
        });
   
    //壁モデルの読み取り
    std::shared_ptr<Model> wallModelFloor = Model::CreateFromCMO(device, ResourceManager::GetModelPath("WallFloor").c_str(), *fx);

    if (wallModel == nullptr){
        assert(wallModel);
    }
    // 下記が無いと白くなりすぎる
    wallModelFloor->UpdateEffects([&](IEffect* effect) {
        BasicEffect* basic = dynamic_cast<BasicEffect*>(effect);
        if (basic != nullptr) {
            basic->SetDiffuseColor(Colors::White);

        }
        });


    // ひびの壁
    std::shared_ptr<Model> wllcrack = Model::CreateFromCMO(device, ResourceManager::GetModelPath("CrackWall").c_str(), *fx);


    // 壁の情報を処理する
    for (int i = 0; i < map.size(); ++i) 
    {
        DirectX::SimpleMath::Vector3 position = map[i];  // 壁の位置
        int typeValue = typemap[i];                      // 壁の種類


        // 壁の種類を判定
        Wall::WallType wallType = Wall::WallType::NONE;  // デフォルト値
        std::shared_ptr<DirectX::Model> model;

        if (typeValue == 1) {
            wallType = Wall::WallType::Indestructible;
            model = wallModel;
        }
        else if (typeValue == 2) {
            wallType = Wall::WallType::Destructible;
            model = wllcrack;
        }
        else if (typeValue == 3)
        {
            wallType = WallType::Indestructible;
            model = wallModelFloor;
        }
        else {
            continue; // 無効な壁タイプの場合スキップ
        }

        // 壁を生成する
        auto newWall = std::make_unique<Wall>(wallType, model, position);
        // 壁の初期化
        newWall->Initialize(commonResources);
        // ベクターに追加
        walls.push_back(std::move(newWall));
    }

    // 完成した壁リストを返す
    return walls;
}
