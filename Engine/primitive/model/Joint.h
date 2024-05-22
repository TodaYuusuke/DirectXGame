#pragma once
#include "object/TransformQuat.h"
#include "Node.h"

#include <vector>
#include <optional>
#include <string>


namespace LWP::Primitive {
	/// <summary>
	/// Animationを行うNode
	/// </summary>
	class Joint {
	public: // ** パブリックなメンバ変数 ** //

		// トランスフォーム（クォータニオン）
		Object::TransformQuat transform;
		// skeletonSpaceでの変換行列
		Math::Matrix4x4 skeletonSpaceMatrix;
		// 名前
		std::string name = "";
		// 子JointのIndexリスト、いなければ空
		std::vector<int32_t> children;
		// 自分のIndex
		int32_t index;
		// 親JointのIndex,いなければnull
		std::optional<int32_t> parent;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Joint() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~Joint() = default;

		/// <summary>
		/// LocalMatrixを求める関数
		/// </summary>
		/// <returns></returns>
		Math::Matrix4x4 GetLocalMatrix() const;

		/// <summary>
		/// NodeクラスからJointを作り出す関数
		/// </summary>
		static int32_t Create(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

		/// <summary>
		/// NodeクラスからJointを作り出す関数
		/// </summary>
		//void Load(aiBone* bone);
	};
}