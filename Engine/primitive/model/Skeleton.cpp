#include "Skeleton.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Primitive;

void Skeleton::Update() {
	// 全てのJointを更新、親が若いので通常ループで処理可能になっている
	for (Joint& joint : joints) {
		Matrix4x4 local = joint.GetLocalMatrix();
		// 親がいれば親の行列を掛ける
		if (joint.parent) {
			joint.skeletonSpaceMatrix = local * joints[*joint.parent].skeletonSpaceMatrix;
		}
		// 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
		else {
			joint.skeletonSpaceMatrix = local;
		}
	}
}

void Skeleton::Create(const Node& node) {
	// 階層構造作成開始
	root = Joint::Create(node, {}, joints);

	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : joints) {
		jointMap.emplace(joint.name, joint.index);
	}
}