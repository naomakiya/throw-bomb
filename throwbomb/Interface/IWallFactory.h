#pragma once
#ifndef IWALLFACTORY_DEFINED
#define IWALLFACTORY_DEFINED
#include <vector>
#include <memory>
#include <Game/Wall/Wall.h>
#include <SimpleMath.h>


class IWallFactory :public Wall
{
	// 指定された壁配列を生成する
	virtual std::vector<std::unique_ptr<Wall>> CreateWalls(CommonResources* commonResources, const char* filename) = 0;

};
#endif