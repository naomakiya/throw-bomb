/*
  @file  WallFactory.h
  @brief ï«çHèÍÉNÉâÉX
*/

#pragma once
#ifndef WALL_FACTORY_DEFINED
#define WALL_FACTORY_DEFINED
#include "Interface/IWallFactory.h"

class WallFactory :	public Wall
{
public:
	// ï«ÇÃê∂ê¨çHèÍ
	static std::vector < std::unique_ptr<Wall> > CreateWalls(CommonResources* commonResources, std::vector<int> typeMap, std::vector<DirectX::SimpleMath::Vector3> posMap);
};

#endif	// BULLET_FACTORY_DEFINED
