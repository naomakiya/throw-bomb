/*
  @file  StageResource.cpp
  @brief ステージリソースクラス
*/
#include "pch.h"
#include "StageResource.h"

void StageResource::AddResource(const std::string& name, const std::string& path)
{
    m_resources[name] = path;
}

std::string StageResource::GetResource(const std::string& name) const
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
