#include "Node.h"
#include <assimp/scene.h>

#include "base/ImGuiManager.h"

using namespace LWP::Primitive;
using namespace LWP::Math;

Node::Node() {
	localMatrix = Matrix4x4::CreateIdentity4x4();
}

void Node::ReadNode(aiNode* node) {
	aiMatrix4x4 ailocalMatrix = node->mTransformation;	// nodeのlocalMatrixを取得
	ailocalMatrix.Transpose();	// 列ベクトルを行ベクトル形式に転置

	localMatrix.m[0][0] = ailocalMatrix[0][0];
	localMatrix.m[0][1] = ailocalMatrix[0][1];
	localMatrix.m[0][2] = ailocalMatrix[0][2];
	localMatrix.m[0][3] = ailocalMatrix[0][3];
	localMatrix.m[1][0] = ailocalMatrix[1][0];
	localMatrix.m[1][1] = ailocalMatrix[1][1];
	localMatrix.m[1][2] = ailocalMatrix[1][2];
	localMatrix.m[1][3] = ailocalMatrix[1][3];
	localMatrix.m[2][0] = ailocalMatrix[2][0];
	localMatrix.m[2][1] = ailocalMatrix[2][1];
	localMatrix.m[2][2] = ailocalMatrix[2][2];
	localMatrix.m[2][3] = ailocalMatrix[2][3];
	localMatrix.m[3][0] = ailocalMatrix[3][0];
	localMatrix.m[3][1] = ailocalMatrix[3][1];
	localMatrix.m[3][2] = ailocalMatrix[3][2];
	localMatrix.m[3][3] = ailocalMatrix[3][3];

	name = node->mName.C_Str();	// Node名を格納
	children.resize(node->mNumChildren);	// 子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
		// 再帰的に読んで階層構造を作っていく
		children[childIndex].ReadNode(node->mChildren[childIndex]);
	}
}
