/*
  @file  WallFactory.h
  @brief �ǍH��N���X
*/

#pragma once
#ifndef WALL_FACTORY_DEFINED
#define WALL_FACTORY_DEFINED
#include "Interface/IWallFactory.h"

class WallFactory :	public Wall
{
public:
	// �ǂ̐����H��
	static std::vector < std::unique_ptr<Wall> > CreateWalls(CommonResources* commonResources, std::vector<int> typeMap, std::vector<DirectX::SimpleMath::Vector3> posMap);
};

#endif	// BULLET_FACTORY_DEFINED
