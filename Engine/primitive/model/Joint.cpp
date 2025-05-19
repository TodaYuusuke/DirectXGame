#include "Joint.h"
#include <assimp/scene.h>

#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Utility;

Math::Matrix4x4 Joint::GetLocalMatrix() const { return localTF.GetAffineMatrix(); }

int32_t Joint::Create(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint& joint = joints.emplace_back();	// SkeletonのJoint列に追加
	joint.name = node.name;
	joint.skeletonSpaceMatrix = Matrix4x4::CreateIdentity4x4();
	joint.localTF = node.transform;
	joint.localTF = node.transform;
	joint.parent = parent;
	int32_t newIndex = int32_t(joints.size() - 1);	// 現在登録されている数をIndexに
	joint.index = newIndex;

	for (const Node& child : node.children) {
		// 子Jointを作成し、そのIndexを登録
		int32_t childIndex = Create(child, newIndex, joints);
		joints[newIndex].children.push_back(childIndex);
	}

	// 自身のIndexを返す
	return newIndex;
}