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
#include "ResultParameteResource.h"

using json = nlohmann::json;

// ���\�[�X�}�l�[�W���[�N���X
class ResourceManager
{
public:
    
    ModelResource& GetModelResource() { return m_modelResource; }
    TextureResource& GetTextureResource() { return m_textureResource; }
    BGMResource& GetBGMResource() { return m_bgmResource; }
    SEResource& GetSEResource() { return m_seResource; }
    StageResource& GetStageResource() { return m_stageResource; }
    ResultParameteResource& GetResultParamete(){ return m_resultParameteResource; }
    

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
                    m_modelResource.AddResource(name, path);
                }
                else if (type == "Texture") {
                    m_textureResource.AddResource(name, path);
                }
                else if (type == "BGM") {
                    m_bgmResource.AddResource(name, path);
                }
                else if (type == "SE") {
                    m_seResource.AddResource(name, path);
                }
                else if (type == "Stage") {
                    m_stageResource.AddResource(name, path);
                }
                else if (type == "ResultParamete"){

                }
            }
        }
    }

    // �w�肵�����f���̃p�X���擾���郁�\�b�h��ǉ�
    static std::wstring GetModelPath(const std::string& name) 
    {
        std::string path = m_modelResource.GetResource(name);
        return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵���e�N�X�`���[�̃p�X���擾���郁�\�b�h��ǉ�
    static std::wstring GetTexturePath(const std::string& name) 
    {
        std::string path = m_textureResource.GetResource(name);
        return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵��BGM�̃p�X���擾���郁�\�b�h��ǉ�
    static std::string GetBGMPath(const std::string& name)
    {
        return m_bgmResource.GetResource(name);
        //return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵��SE�̃p�X���擾���郁�\�b�h��ǉ�
    static std::string GetSEPath(const std::string& name)
    {
        return m_seResource.GetResource(name);
        //return std::wstring(path.begin(), path.end());  // std::string ���� std::wstring �ւ̕ϊ�
    }
    // �w�肵��SE�̃p�X���擾���郁�\�b�h��ǉ�
    static std::string GetStagePath(const std::string& name)
    {
        return m_stageResource.GetResource(name);
    }

private:
    // �V���O���g���p�̐ÓI�ȃC���X�^���X
    static std::unique_ptr<ResourceManager> instance ;
    static std::mutex mutex;

    static ModelResource m_modelResource;
    static TextureResource m_textureResource;
    static BGMResource m_bgmResource;
    static SEResource m_seResource;
    static StageResource m_stageResource;
    static ResultParameteResource m_resultParameteResource;
};