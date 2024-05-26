#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>

// 前方宣言
namespace LWP::Object::Collider {
	class AABB;
}

namespace LWP::Primitive {
	/// <summary>
	/// 立方体
	/// </summary>
	class oldCube final
		: public IPrimitive {
	public: // ** 関数 ** //

		// 初期化を呼び出す
		Cube() { Init(); }

		/// <summary>
		/// 頂点を生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		void CreateVertices() override;
		void CreateVertices(const LWP::Math::Vector3& minSize, const LWP::Math::Vector3& maxSize);
		/// <summary>
		/// インデックスを生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		void CreateIndexes() override;

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const override;
		/// <summary>
		/// インデックスの数を返す関数
		/// </summary>
		int GetIndexCount() const override;
		
		/// <summary>
		/// AABBからCubeを生成する関数
		/// </summary>
		/// <param name="aabb">作成するAABB</param>
		void CreateFromAABB(const LWP::Object::Collider::AABB& aabb);


	private: // ** プライベートな関数 ** //

		/// <summary>
		/// ImGui
		/// </summary>
		void DerivedDebugGUI(const std::string& label = "Derived") override;
	};
}