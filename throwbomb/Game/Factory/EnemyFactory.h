/*
	@file	EnemyFactory.h
	@brief	õ“G‚·‚é“G‚ÌHêƒNƒ‰ƒX
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
private:
};

#endif	// ENEMY_FACTORY_DEFINED
