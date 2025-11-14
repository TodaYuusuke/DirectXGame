#include "JointWeight.h"

#include <assimp/scene.h>
#include "math/Math.h"
#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Primitive;

void JointWeight::Load(aiBone* bone) {
	// InverseBindPoseMatrixの抽出
	aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
	aiVector3D scale, translate;
	aiQuaternion rotate;
	bindPoseMatrixAssimp.Decompose(scale, rotate, translate);
	Matrix4x4 bindPoseMatrix = Matrix4x4::CreateAffineMatrix(
		Vector3{ scale.x,scale.y, scale.z },
		Quaternion{ rotate.x,-rotate.y, -rotate.z,rotate.w },
		Vector3{ -translate.x,translate.y, translate.z }
	);
	inverseBindPoseMatrix = bindPoseMatrix.Inverse();

	// Weight情報を取り出す
	for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++) {
		vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
	}
}