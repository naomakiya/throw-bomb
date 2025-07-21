/*
	@file	EnemyFactory.cpp
	@brief	���G�@�\�����G�𐶐�����t�@�N�g���[�N���X
*/

#include "pch.h"
#include "Game/Enemy/EnemyState.h"
#include "Game/Factory/EnemyFactory.h"

std::vector<std::unique_ptr<IEnemyState>> EnemyFactory::CreateEnemy( const std::vector<std::unique_ptr<Wall>>& wall, 
	PlayerState* player, CommonResources* commonResources, 
	std::vector <DirectX::SimpleMath::Vector3> map, 
	const std::vector<DirectX::SimpleMath::Vector3> patrolPath)
{
	using namespace DirectX::SimpleMath;

	// �z���錾����
	std::vector<std::unique_ptr<IEnemyState>> enemies;
	// �e�X�|�[�����W���ƂɓG����
	for (const auto& enemyPosition : map){
		// �G�l�~�[�̍쐬
		std::unique_ptr<EnemyState> enemy = std::make_unique<EnemyState>(wall, player, patrolPath, enemyPosition);
		enemy->Initialize(commonResources);
		// �z��ɑ}��
		enemies.push_back(std::move(enemy));
	}
	// �G�z���Ԃ�
	return enemies;
}
