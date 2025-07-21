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
	std::vector<std::unique_ptr<IEnemyState>> enemies;
	// 各スポーン座標ごとに敵生成
	for (const auto& mapPosition : map){
		// 突進する敵の作成
		std::unique_ptr<StraighteningEnemyState> enemy = std::make_unique<StraighteningEnemyState>(wall, player, mapPosition);
		enemy->Initialize(commonResources);
		// 配列に挿入
		enemies.push_back(std::move(enemy));
	}
	// 配列を返す
	return enemies;
}
