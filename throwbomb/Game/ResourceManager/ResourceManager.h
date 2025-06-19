/*
    @file	ResourceManager.h
    @brief	���\�[�X�p�X�Ǘ��N���X
*/
#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <mutex>
#include "ModelResource.h"
#include "TextureResource.h"
#include "BGMResource.h"
#include "SEResource.h"
#include "StageResource.h"
using json = nlohmann::json;

// ���\�[�X�}�l�[�W���[�N���X
class ResourceManager
{
public:
    
    ModelResource& getModelResource() { return modelResource_; }
    TextureResource& getTextureResource() { return textureResource_; }
    BGMResource& getBGMResource() { return bgmResource_; }
    SEResource& getSEResource() { return seResource_; }
    StageResource& getStageResource() { return stageResource; }
    

    // �V���O���g���C���X�^���X�̎擾
    static ResourceManager& GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance)
        {
            instance.reset(new ResourceManager());
        }
        return *instance;
    }
    //���\�[�X�W�F�C�\���̎擾
    void loadFromJson(const std::string& filePath) {
        std::ifstream file(filePath);
        json data;
        file >> data;

        for (const auto& typeResourcesPair : data.items()) {
            // ���\�[�X�^�C�v�i�Ⴆ�� "Model" �� "Texture"�j
            std::string type = typeResourcesPair.key();

            // �e���\�[�X�̃f�[�^�iname��path�̃y�A�j�� json �^�Ƃ��Ď擾
            const json& resources = typeResourcesPair.value();

            // name �� path �̃y�A�𔽕�����
            for (const auto& namePathPair : resources.items()) {
                std::string name = namePathPair.key();         // ���\�[�X�̖��O�i��: "Floor"�j
                std::string path = namePathPair.value();       // ���\�[�X�̃p�X�i��: "Model/FloorObject"�j

                if (type == "Model") {
                    modelResource_.addResource(name, path);
                }
                else if (type == "Texture") {
                    textureResource_.addResource(name, path);
                }
                else if (type == "BGM") {
                    bgmResource_.addResource(name, path);
                }
                else if (type == "SE") {
                    seResource_.addResource(name, path);
                }
                else if (type == "Stage") {
                    stageResource.addResource(name, path);
                }
            }
        }
    }

    // �w�肵�����f���̃p�X���擾���郁�\�b�h��ǉ�
    static std::wstring getModelPath(const std::string& name) 
    {
        std::string path = modelResource_.getResource(name);
        return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵���e�N�X�`���[�̃p�X���擾���郁�\�b�h��ǉ�
    static std::wstring getTexturePath(const std::string& name) 
    {
        std::string path = textureResource_.getResource(name);
        return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵��BGM�̃p�X���擾���郁�\�b�h��ǉ�
    static std::string getBGMPath(const std::string& name)
    {
        return bgmResource_.getResource(name);
        //return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵��SE�̃p�X���擾���郁�\�b�h��ǉ�
    static std::wstring getSEPath(const std::string& name)
    {
        std::string path = seResource_.getResource(name);
        return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵��SE�̃p�X���擾���郁�\�b�h��ǉ�
    static std::string getStagePath(const std::string& name)
    {
        return stageResource.getResource(name);
    }

private:
    // �V���O���g���p�̐ÓI�ȃC���X�^���X
    static std::unique_ptr<ResourceManager> instance ;
    static std::mutex mutex;

    static ModelResource modelResource_;
    static TextureResource textureResource_;
    static BGMResource bgmResource_;
    static SEResource seResource_;
    static StageResource stageResource;
};