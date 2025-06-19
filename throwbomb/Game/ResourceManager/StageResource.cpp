/*
  @file  StageResource.cpp
  @brief BGMリソースマネージャークラス
*/
#include "pch.h"
#include "StageResource.h"

void StageResource::addResource(const std::string& name, const std::string& path)
{
    m_resources[name] = path;
}

std::string StageResource::getResource(const std::string& name) const
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
