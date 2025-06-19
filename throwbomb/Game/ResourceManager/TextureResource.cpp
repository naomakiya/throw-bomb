/*
  @file TextureResource.cpp
  @brief リソースマネージャークラス
*/
#include "pch.h"
#include "TextureResource.h"

void TextureResource::addResource(const std::string& name, const std::string& path) 
{
    m_resources[name] = path;
}

std::string TextureResource::getResource(const std::string& name) const 
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
