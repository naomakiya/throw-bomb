/*
    @file	ModelResource.h
    @brief	モデルリソースパス管理クラス
*/
#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <mutex>

class ModelResource {
    public:
    void AddResource(const std::string& name, const std::string& path);
    std::string GetResource(const std::string& name) const;

    private:
    std::unordered_map<std::string, std::string> m_resources;
};
