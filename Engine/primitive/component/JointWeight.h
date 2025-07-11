#pragma once
#include "math/matrix/Matrix4x4.h"
#include <vector>

// assimpの読み込み
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace LWP::Primitive {
	/// <summary>
	/// 頂点の重み
	/// </summary>
	struct VertexWeight {
		float weight;
		uint32_t vertexIndex;
	};

	/// <summary>
	/// Jointの重み
	/// </summary>
	class JointWeight {
	public: // ** パブリックなメンバ変数 ** //
	
		Math::Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeight> vertexWeights;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		JointWeight() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~JointWeight() = default;

		/// <summary>
		/// NodeクラスからJointを作り出す関数
		/// </summary>
		void Load(aiBone* bone);
	};
}