/*
	@file	EnemyFactory.cpp
	@brief	索敵機能を持つ敵を生成するファクトリークラス
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

	// 配列を宣言する
	std::vector<std::unique_ptr<IEnemyState>> enemies;
	// 各スポーン座標ごとに敵生成
	for (const auto& enemyPosition : map){
		// エネミーの作成
		std::unique_ptr<EnemyState> enemy = std::make_unique<EnemyState>(wall, player, patrolPath, enemyPosition);
		enemy->Initialize(commonResources);
		// 配列に挿入
		enemies.push_back(std::move(enemy));
	}
	// 敵配列を返す
	return enemies;
}
