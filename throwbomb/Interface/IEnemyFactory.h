#pragma once
#ifndef IENEMYFACTORY_DEFINED
#define IENEMYFACTORY_DEFINED
#include <vector>
#include"Interface/IEnemyState.h"
#include <memory>

class IEnemyFactory : public IEnemyState
{
	// �w�肳�ꂽ�C�e�z��𐶐�����
	virtual std::vector<std::unique_ptr<IEnemyState>> CreateEnemys(
		 const bool enemyExist) = 0;

};
#endif