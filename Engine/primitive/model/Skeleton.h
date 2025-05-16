#pragma once
#include "Joint.h"
#include <map>

namespace LWP::Primitive {
	/// <summary>
	/// Jointの階層構造を管理する機構
	/// </summary>
	class Skeleton {
	public: // ** パブリックなメンバ変数 ** //

		// RootJointのIndex
		int32_t root{};
		// Joint名とIndexとの辞書
		std::map<std::string, int32_t> jointMap{};
		// 所属しているJoint
		std::vector<Joint> joints{};


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Skeleton() = default;
		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~Skeleton() = default;

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

		/// <summary>
		/// Nodeクラスからスケルトンを作り出す関数
		/// </summary>
		/// <param name="material"></param>
		void Create(const Node& node);
		/// <summary>
		/// Joint同士の親子関係をセットする関数
		/// </summary>
		void SetParent();
	};
}