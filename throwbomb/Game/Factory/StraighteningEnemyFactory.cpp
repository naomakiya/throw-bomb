/*
	@file	StraighteningEnemyState.cpp
	@brief	突進する敵の工場クラス
*/
#include "pch.h"
#include "Game/Enemy/StraighteningEnemyState.h"
#include "Game/Factory/StraighteningEnemyFactory.h"

std::vector<std::unique_ptr<IEnemyState>> StraighteningEnemyFactory::CreateEnemy( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, CommonResources* commonResources, std::vector <DirectX::SimpleMath::Vector3> map, const std::vector<DirectX::SimpleMath::Vector3> patrolPath)
{
	using namespace DirectX::SimpleMath;

	// 配列を宣言する
	std::vector<std::unique_ptr<IEnemyState>> enemys;

	for (const auto& maps : map)
	{
		DirectX::SimpleMath::Vector3 enemyPos = maps;

		std::unique_ptr<StraighteningEnemyState> enemy = std::make_unique<StraighteningEnemyState>(wall, player);
		enemy->Initialize(commonResources, enemyPos);

		enemys.push_back(std::move(enemy));
	}
	// てき配列を返す
	return enemys;
}
