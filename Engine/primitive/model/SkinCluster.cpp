#include "SkinCluster.h"
#include <assimp/scene.h>

#include "base/ImGuiManager.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Primitive;

SkinCluster::SkinCluster(uint32_t size) {
	mappedPalette.resize(size);

	inverseBindPoseMatrices.resize(size);
	for (int i = 0; i < inverseBindPoseMatrices.size(); i++) {
		inverseBindPoseMatrices[i] = Math::Matrix4x4::CreateIdentity4x4();
	}
};