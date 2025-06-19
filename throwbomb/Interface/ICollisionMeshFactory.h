#pragma once
#ifndef ICOLLISIONMESHFACTORY_DEFINED
#define ICOLLISIONMESHFACTORY_DEFINED
#include <vector>
#include <memory>
#include <Libraries/MyLib/CollisionMesh.h>
#include <SimpleMath.h>

class  CommonResources;;

namespace mylib
{
	class ICollisionMeshFactory : public CollisionMesh
	{
		// �w�肳�ꂽ�ǔz��𐶐�����
		virtual std::vector<std::unique_ptr<CollisionMesh>> CreateCollisionMeshs(CommonResources* commonResources, const char* filename) = 0;

	};
}
#endif