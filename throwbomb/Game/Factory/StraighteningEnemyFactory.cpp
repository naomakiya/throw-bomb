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
	std::vector<std::unique_ptr<IEnemyState>> enemies;
	// �e�X�|�[�����W���ƂɓG����
	for (const auto& mapPosition : map){
		// �ːi����G�̍쐬
		std::unique_ptr<StraighteningEnemyState> enemy = std::make_unique<StraighteningEnemyState>(wall, player, mapPosition);
		enemy->Initialize(commonResources);
		// �z��ɑ}��
		enemies.push_back(std::move(enemy));
	}
	// �z���Ԃ�
	return enemies;
}
