/*
  @file  CollisionMeshFactory.h
  @brief ç‚çHèÍÉNÉâÉX
*/
#pragma once
#ifndef COLLISIONMESH_FACTORY_DEFINED
#define COLLISIONMESH_FACTORY_DEFINED
#include <vector>
#include "Interface/ICollisionMeshFactory.h"

class CommonResources;

class CollisionMeshFactory :	public mylib::CollisionMesh
{
public:
	static std::vector < std::unique_ptr<mylib::CollisionMesh>> CreateCollisionMeshs(
		CommonResources* commonResources, 
		std::vector<int> typemap,
		std::vector<DirectX::SimpleMath::Vector3> posMap,
		std::vector < DirectX::SimpleMath::Quaternion> mapRotatio);

};

#endif	// BULLET_FACTORY_DEFINED
