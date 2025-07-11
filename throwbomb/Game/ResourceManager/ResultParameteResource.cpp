/*
  @file  ResultParameteResource.cpp
  @brief リザルトリソースクラス
*/
#include "pch.h"
#include "ResultParameteResource.h"

void ResultParameteResource::AddResource(const std::string& name, const std::string& path)
{
    m_resources[name] = path;
}

std::string ResultParameteResource::GetResource(const std::string& name) const
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
