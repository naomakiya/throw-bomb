/*
  @file Resource.cpp
  @brief ���\�[�X�}�l�[�W���[�N���X
*/
#include "pch.h"
#include"Game/ResourceManager/ResourceManager.h"
#include <cassert>
#include <iostream>

// �ÓI�����o�ϐ��̒�`�i�������K�v�I�j
std::unique_ptr<ResourceManager> ResourceManager::instance = nullptr;
std::mutex ResourceManager::mutex;

ModelResource ResourceManager::m_modelResource;
TextureResource ResourceManager::m_textureResource;
BGMResource ResourceManager::m_bgmResource;
SEResource ResourceManager::m_seResource;
StageResource ResourceManager::m_stageResource;