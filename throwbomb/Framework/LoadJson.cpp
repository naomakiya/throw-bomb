#include "pch.h"
#include "LoadJson.h"

#include <fstream>
#include <assert.h>
#include <iostream>

#include"nlohmann/json.hpp"
using json = nlohmann::json;

// コンストラクタ: ファイルから JSON をロードする
LoadJson::LoadJson(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON file: " << filename << std::endl;
        return;
    }
    try 
    {
        file >> m_jsonData;
        m_loaded = true;
    }
    catch (const json::parse_error& e) 
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        m_loaded = false;
    }
}



void LoadJson::SaveToFile(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    file << m_jsonData.dump(4); // 4はインデント（見やすい整形）
    file.close();
}

void LoadJson::SetVolume(const std::string& category, float volume)
{
    m_jsonData[category]["Volume"] = volume;
}

// オブジェクトデータを取得する
std::vector<ObjectData> LoadJson::GetObjects(const char* object) const
{
    std::vector<ObjectData> objects;
    // ロードに失敗していたら空のまま返す
    if (!m_loaded) return objects; 

    for (const auto& entry : m_jsonData[object])
    {
        ObjectData data;
        if (entry.contains("type"))
        {
            data.type = entry.value("type", 0); // "type" を取得
        }

        if (entry.contains("position"))
        {
            auto& position = entry["position"];
            data.position = {
                position.value("x", 0.0f),
                position.value("y", 0.0f),
                position.value("z", 0.0f)
            };
        }

        if (entry.contains("rotation"))
        {
            float rotationDegrees = entry.value("rotation", 0.0f);
            data.rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
                DirectX::XMConvertToRadians(rotationDegrees), 0.0f, 0.0f
            );
        }
        if (entry.contains("Volume"))
        {
            data.volume = entry.value("Volume", 0.0f);
        }
       
        objects.push_back(data);
    }

    return objects;
}