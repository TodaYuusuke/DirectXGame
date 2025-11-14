#pragma once
#include "IPrimitive3D.h"

namespace LWP::Primitive {
	/// <summary>
	/// 球
	/// </summary>
	class Sphere
		: public IPrimitive3D {
	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sphere();
		/// <summary>
		/// タイプを返す
		/// </summary>
		Type GetType() { return Type::Sphere; }

		/// <summary>
		/// Sphereコライダーから描画用のスフィアを生成
		/// </summary>
		/// <param name="pos">中心</param>
		/// <param name="rad">半径</param>
		void CreateFromSphereCol(const Math::Vector3& pos, const float& rad);
	};
}