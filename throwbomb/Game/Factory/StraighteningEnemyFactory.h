/*
	@file	StraighteningEnemyState.h
	@brief	ìÀêiÇ∑ÇÈìGÇÃçHèÍÉNÉâÉX
*/
#pragma once
#ifndef STRAIGHTENINGENEMY_FACTORY_DEFINED
#define STRAIGHTENINGENEMY_FACTORY_DEFINED
#include "Interface/IEnemyFactory.h"

class StraighteningEnemyState;

class StraighteningEnemyFactory : public IEnemyFactory
{
public:
	static std::vector<std::unique_ptr<IEnemyState>>CreateEnemy( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, CommonResources* commonResources, std::vector <DirectX::SimpleMath::Vector3> map, const std::vector<DirectX::SimpleMath::Vector3> patrolPath);
private:
};

#endif	// STRAIGHTENINGENEMY_FACTORY_DEFINED
