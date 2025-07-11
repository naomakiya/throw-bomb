/*
  @file  ModelResource.cpp
  @brief モデルリソースクラス
*/
#include "pch.h"
#include "ModelResource.h"

void ModelResource::AddResource(const std::string& name, const std::string& path) 
{
    m_resources[name] = path;
}

std::string ModelResource::GetResource(const std::string& name) const
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
