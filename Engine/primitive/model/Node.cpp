#include "Node.h"
#include <assimp/scene.h>

#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Primitive;

void Node::Load(aiNode* node) {
	aiVector3D scale, translate;
	aiQuaternion rotate;
	// assimpの行列からSRTを抽出する関数を利用
	node->mTransformation.Decompose(scale, rotate, translate);

	transform.scale = { scale.x,scale.y, scale.z };	// Scaleはそのまま
	transform.rotation = { rotate.x,-rotate.y,-rotate.z,rotate.w };	// X軸を反転、さらに回転方向が逆なので軸を反転させる
	transform.translation = { -translate.x,translate.y ,translate.z };	// X軸を反転

	name = node->mName.C_Str();	// Node名を格納
	children.resize(node->mNumChildren);	// 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		// 再帰的に読んで階層構造を作っていく
		children[childIndex].Load(node->mChildren[childIndex]);
	}
}

Math::Matrix4x4 Node::GetLocalMatrix() const { return transform.GetAffineMatrix(); }

