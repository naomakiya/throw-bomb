/*
  @file SEResource.cpp
  @brief 効果音リソースクラス
*/
#include "pch.h"
#include "SEResource.h"

void SEResource::AddResource(const std::string& name, const std::string& path)
{
    m_resources[name] = path;
}

std::string SEResource::GetResource(const std::string& name) const
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
