/*
  @file Resource.cpp
  @brief リソースマネージャークラス
*/
#include "pch.h"
#include"Game/ResourceManager/ResourceManager.h"
#include <cassert>
#include <iostream>

// 静的メンバ変数の定義（ここが必要！）
std::unique_ptr<ResourceManager> ResourceManager::instance = nullptr;
std::mutex ResourceManager::mutex;

ModelResource ResourceManager::m_modelResource;
TextureResource ResourceManager::m_textureResource;
BGMResource ResourceManager::m_bgmResource;
SEResource ResourceManager::m_seResource;
StageResource ResourceManager::m_stageResource;