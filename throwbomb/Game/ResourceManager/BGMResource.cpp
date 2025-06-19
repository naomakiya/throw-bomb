/*
  @file  BGMResource.cpp
  @brief BGMリソースマネージャークラス
*/
#include "pch.h"
#include "BGMResource.h"

void BGMResource::addResource(const std::string& name, const std::string& path)
{
    m_resources[name] = path;
}

std::string BGMResource::getResource(const std::string& name) const
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
