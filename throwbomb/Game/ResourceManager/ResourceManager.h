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
using json = nlohmann::json;

// リソースマネージャークラス
class ResourceManager
{
public:
    
    ModelResource& getModelResource() { return modelResource_; }
    TextureResource& getTextureResource() { return textureResource_; }
    BGMResource& getBGMResource() { return bgmResource_; }
    SEResource& getSEResource() { return seResource_; }
    StageResource& getStageResource() { return stageResource; }
    

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

    // 指定したモデルのパスを取得するメソッドを追加
    static std::wstring getModelPath(const std::string& name) 
    {
        std::string path = modelResource_.getResource(name);
        return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したテクスチャーのパスを取得するメソッドを追加
    static std::wstring getTexturePath(const std::string& name) 
    {
        std::string path = textureResource_.getResource(name);
        return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したBGMのパスを取得するメソッドを追加
    static std::string getBGMPath(const std::string& name)
    {
        return bgmResource_.getResource(name);
        //return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したSEのパスを取得するメソッドを追加
    static std::wstring getSEPath(const std::string& name)
    {
        std::string path = seResource_.getResource(name);
        return std::wstring(path.begin(), path.end());  // std::string から std::wstring への変換
    }
    // 指定したSEのパスを取得するメソッドを追加
    static std::string getStagePath(const std::string& name)
    {
        return stageResource.getResource(name);
    }

private:
    // シングルトン用の静的なインスタンス
    static std::unique_ptr<ResourceManager> instance ;
    static std::mutex mutex;

    static ModelResource modelResource_;
    static TextureResource textureResource_;
    static BGMResource bgmResource_;
    static SEResource seResource_;
    static StageResource stageResource;
};