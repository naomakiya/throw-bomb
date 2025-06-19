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
		// éwíËÇ≥ÇÍÇΩï«îzóÒÇê∂ê¨Ç∑ÇÈ
		virtual std::vector<std::unique_ptr<CollisionMesh>> CreateCollisionMeshs(CommonResources* commonResources, const char* filename) = 0;

	};
}
#endif