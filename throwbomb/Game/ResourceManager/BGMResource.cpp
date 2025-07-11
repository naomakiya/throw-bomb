/*
  @file  BGMResource.cpp
  @brief BGM���\�[�X�}�l�[�W���[�N���X
*/
#include "pch.h"
#include "BGMResource.h"

void BGMResource::AddResource(const std::string& name, const std::string& path)
{
    m_resources[name] = path;
}

std::string BGMResource::GetResource(const std::string& name) const
{
    auto it = m_resources.find(name);
    return (it != m_resources.end()) ? it->second : "";
}
