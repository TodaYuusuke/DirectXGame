#pragma once
#include "base/directX/resource/data/StructuredBuffer.h"
#include "object/TransformQuat.h"
#include "Node.h"

#include <array>
#include <vector>

namespace LWP::Primitive {
	const uint32_t kNumMaxInfluence = 4;
	struct WellForGPU {
		Math::Matrix4x4 skeletonSpaceMatrix;	// 位置用
		Math::Matrix4x4 skeletonSpaceInverseTransposeMatrix;	// 法線用
	};

	/// <summary>
	/// SkinClusterクラス
	/// </summary>
	class SkinCluster {
	public: // ** パブリックなメンバ変数 ** //

		// JointのInverseBindPoseMatrixがIndex順に保存されているだけのもの
		std::vector<Math::Matrix4x4> inverseBindPoseMatrices;
		//std::vector<VertexInfluence> mappedInfluence;
		std::vector<WellForGPU> mappedPalette;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SkinCluster() = delete;
		SkinCluster(uint32_t size);
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~SkinCluster() = default;
	};
}