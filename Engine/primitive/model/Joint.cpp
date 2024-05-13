#include "Joint.h"
#include <assimp/scene.h>

#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Primitive;

Math::Matrix4x4 Joint::GetLocalMatrix() const { return transform.GetAffineMatrix(); }

int32_t Joint::Create(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.skeletonSpaceMatrix = Matrix4x4::CreateIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size());	// 現在登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint);	// SkeletonのJoint列に追加
	for (const Node& child : node.children) {
		// 子Jointを作成し、そのIndexを登録
		int32_t childIndex = Create(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}

	// 自身のIndexを返す
	return joint.index;
}