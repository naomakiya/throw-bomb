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

ModelResource ResourceManager::modelResource_;
TextureResource ResourceManager::textureResource_;
BGMResource ResourceManager::bgmResource_;
SEResource ResourceManager::seResource_;
StageResource ResourceManager::stageResource;