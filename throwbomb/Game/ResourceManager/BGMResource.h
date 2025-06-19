/*
    @file	BGMResource.h
    @brief	BGM�e�N�X�`���[���\�[�X�Ǘ��N���X
*/
#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <mutex>

class BGMResource
{
public:
    void addResource(const std::string& name, const std::string& path);
    std::string getResource(const std::string& name) const;

private:
    std::unordered_map<std::string, std::string> m_resources;
};
