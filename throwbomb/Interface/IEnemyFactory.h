#pragma once
#ifndef IENEMYFACTORY_DEFINED
#define IENEMYFACTORY_DEFINED
#include <vector>
#include"Interface/IEnemyState.h"
#include <memory>

class IEnemyFactory : public IEnemyState
{
	// 指定された砲弾配列を生成する
	virtual std::vector<std::unique_ptr<IEnemyState>> CreateEnemys(
		 const bool enemyExist) = 0;

};
#endif