/*
    @file	ResourceManager.h
    @brief	リソースパス管理クラス
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

// リソースマネージャークラス
class ResourceManager
{
public:
    
    ModelResource& GetModelResource() { return m_modelResource; }
    TextureResource& GetTextureResource() { return m_textureResource; }
    BGMResource& GetBGMResource() { return m_bgmResource; }
    SEResource& GetSEResource() { return m_seResource; }
    StageResource& GetStageResource() { return m_stageResource; }
    ResultParameteResource& GetResultParamete(){ return m_resultParameteResource; }
    

    // シングルトンインスタンスの取得
    static ResourceManager& GetInstance()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance)
        {
            instance.reset(new ResourceManager());
        }
        return *instance;
    }
    //リソースジェイソンの取得
    void loadFromJson(const std::string& filePath) {
        std::ifstream file(filePath);
        json data;
        file >> data;

        for (const auto& typeResourcesPair : data.items()) {
            // リソースタイプ（例えば "Model" や "Texture"）
            std::string type = typeResourcesPair.key();

            // 各リソースのデータ（nameとpathのペア）を json 型として取得
            const json& resources = typeResourcesPair.value();

            // name と path のペアを反復処理
            for (const auto& namePathPair : resources.items()) {
                std::string name = namePathPair.key();         // リソースの名前（例: "Floor"）
                std::string path = namePathPair.value();       // リソースのパス（例: "Model/FloorObject"）

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

    // 指定したモデルのパスを取得するメソッドを追加
    static std::wstring GetModelPath(const std::string& name) 
    {
        std::string path = m_modelResource.GetResource(name);
        return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したテクスチャーのパスを取得するメソッドを追加
    static std::wstring GetTexturePath(const std::string& name) 
    {
        std::string path = m_textureResource.GetResource(name);
        return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したBGMのパスを取得するメソッドを追加
    static std::string GetBGMPath(const std::string& name)
    {
        return m_bgmResource.GetResource(name);
        //return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したSEのパスを取得するメソッドを追加
    static std::string GetSEPath(const std::string& name)
    {
        return m_seResource.GetResource(name);
        //return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したSEのパスを取得するメソッドを追加
    static std::string GetStagePath(const std::string& name)
    {
        return m_stageResource.GetResource(name);
    }

private:
    // シングルトン用の静的なインスタンス
    static std::unique_ptr<ResourceManager> instance ;
    static std::mutex mutex;

    static ModelResource m_modelResource;
    static TextureResource m_textureResource;
    static BGMResource m_bgmResource;
    static SEResource m_seResource;
    static StageResource m_stageResource;
    static ResultParameteResource m_resultParameteResource;
};