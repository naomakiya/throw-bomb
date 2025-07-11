/*
    @file	SEResource.h
    @brief	���ʉ��`���[���\�[�X�p�X�Ǘ��N���X
*/
#pragma once
#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <mutex>

class SEResource
{
public:
    void AddResource(const std::string& name, const std::string& path);
    std::string GetResource(const std::string& name) const;

private:
    std::unordered_map<std::string, std::string> m_resources;
};
