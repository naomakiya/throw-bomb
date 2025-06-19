/// <summary>
/// Jsonファイルを扱うクラス
/// </summary>
#pragma once

#include <memory>

#include"nlohmann/json.hpp"
using json = nlohmann::json;

struct ObjectData 
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Quaternion rotation;
	float volume;
	int type = 0;
};

class LoadJson
{
public:
	bool IsLoaded() const { return m_loaded; }
	std::vector<ObjectData> GetObjects(const char* object) const;
	const json& GetJson() const { return m_jsonData; }
public:
	LoadJson(const std::string& filename);
	void SaveToFile(const std::string& filename) const;
	void SetVolume(const std::string& category, float volume);
private:
	// JSONデータを保持
	json m_jsonData; 
	// ロード成功フラグ
	bool m_loaded = false; 

};