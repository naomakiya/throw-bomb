/*
	@file	EnemyFactory.h
	@brief	索敵機能を持つ敵を生成するファクトリークラス
*/
#pragma once
#ifndef ENEMY_FACTORY_DEFINED
#define ENEMY_FACTORY_DEFINED
#include "Interface/IEnemyFactory.h"

class EnemyState;

class EnemyFactory : public IEnemyFactory
{
public:
	static std::vector<std::unique_ptr<IEnemyState>>CreateEnemy( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, CommonResources* commonResources, std::vector <DirectX::SimpleMath::Vector3> map, const std::vector<DirectX::SimpleMath::Vector3> patrolPath);
};

#endif	// ENEMY_FACTORY_DEFINED
