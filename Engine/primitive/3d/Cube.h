#pragma once
#include "IPrimitive3D.h"

namespace LWP::Primitive {
	/// <summary>
	/// 立方体
	/// </summary>
	class Cube final
		: public IPrimitive3D {
	public: // ** 関数 ** //
		
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Cube();
		/// <summary>
		/// タイプを返す
		/// </summary>
		Type GetType() { return Type::Cube; }

		/// <summary>
		/// AABBからCubeを生成する関数
		/// </summary>
		/// <param name="aabb">作成するAABB</param>
		void CreateFromAABB(const Math::Vector3& min, const Math::Vector3& max);
	};
}