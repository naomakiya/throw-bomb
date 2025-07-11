/*
  @file  WallFactory.cpp
  @brief �ǍH��N���X
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
    // �z���錾����
    std::vector<std::unique_ptr<Wall>> walls ;

    // `typemap` �� `map` �̃T�C�Y����v���Ă��邱�Ƃ��m�F
    if (typemap.size() != map.size()) {
        throw std::invalid_argument("typemap �� map �̃T�C�Y����v���Ă��܂���B");
    }

    //�ǂݍ��ݏ���
    std::unique_ptr<EffectFactory> fx = std::make_unique<EffectFactory>(device);
    fx->SetDirectory(L"Resources/Models");
    //�ǃ��f���̓ǂݎ��
    std::shared_ptr<Model> wallModel = Model::CreateFromCMO(device, ResourceManager::GetModelPath("Wall").c_str(), *fx);
    if (wallModel == nullptr)
    {
        assert(wallModel);
    }
    // ���L�������Ɣ����Ȃ肷����
    wallModel->UpdateEffects([&](IEffect* effect) {
        BasicEffect* basic = dynamic_cast<BasicEffect*>(effect);
        if (basic != nullptr) {
            basic->SetDiffuseColor(Colors::Black);

        }
        });
   
    //�ǃ��f���̓ǂݎ��
    std::shared_ptr<Model> wallModelFloor = Model::CreateFromCMO(device, ResourceManager::GetModelPath("WallFloor").c_str(), *fx);

    if (wallModel == nullptr){
        assert(wallModel);
    }
    // ���L�������Ɣ����Ȃ肷����
    wallModelFloor->UpdateEffects([&](IEffect* effect) {
        BasicEffect* basic = dynamic_cast<BasicEffect*>(effect);
        if (basic != nullptr) {
            basic->SetDiffuseColor(Colors::White);

        }
        });


    // �Ђт̕�
    std::shared_ptr<Model> wllcrack = Model::CreateFromCMO(device, ResourceManager::GetModelPath("CrackWall").c_str(), *fx);


    // �ǂ̏�����������
    for (int i = 0; i < map.size(); ++i) 
    {
        DirectX::SimpleMath::Vector3 position = map[i];  // �ǂ̈ʒu
        int typeValue = typemap[i];                      // �ǂ̎��


        // �ǂ̎�ނ𔻒�
        Wall::WallType wallType = Wall::WallType::NONE;  // �f�t�H���g�l
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
            continue; // �����ȕǃ^�C�v�̏ꍇ�X�L�b�v
        }

        // �ǂ𐶐�����
        auto newWall = std::make_unique<Wall>(wallType, model, position);
        // �ǂ̏�����
        newWall->Initialize(commonResources);
        // �x�N�^�[�ɒǉ�
        walls.push_back(std::move(newWall));
    }

    // ���������ǃ��X�g��Ԃ�
    return walls;
}
