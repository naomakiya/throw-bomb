/*
	@file	StraighteningEnemyState.cpp
	@brief	�ːi����G�̍H��N���X
*/
#include "pch.h"
#include "Game/Enemy/StraighteningEnemyState.h"
#include "Game/Factory/StraighteningEnemyFactory.h"

std::vector<std::unique_ptr<IEnemyState>> StraighteningEnemyFactory::CreateEnemy( const std::vector<std::unique_ptr<Wall>>& wall, PlayerState* player, CommonResources* commonResources, std::vector <DirectX::SimpleMath::Vector3> map, const std::vector<DirectX::SimpleMath::Vector3> patrolPath)
{
	using namespace DirectX::SimpleMath;

	// �z���錾����
	std::vector<std::unique_ptr<IEnemyState>> enemys;

	for (const auto& maps : map)
	{
		DirectX::SimpleMath::Vector3 enemyPos = maps;

		std::unique_ptr<StraighteningEnemyState> enemy = std::make_unique<StraighteningEnemyState>(wall, player);
		enemy->Initialize(commonResources, enemyPos);

		enemys.push_back(std::move(enemy));
	}
	// �Ă��z���Ԃ�
	return enemys;
}
