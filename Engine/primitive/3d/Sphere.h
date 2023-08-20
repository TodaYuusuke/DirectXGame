#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace LWP::Primitive {
	class Surface;

	/// <summary>
	/// 球
	/// </summary>
	class Sphere final
		: public IPrimitive {
	public:
		// コンストラクタ
		Sphere(Manager*);


		// ~ 定数 ~ //
		const uint32_t kSubdivision = 16;									// 分割数
		const float kLonEvery = (float)(2.0f * M_PI) / (float)kSubdivision;	// 経度分割1つ分の角度
		const float kLatEvery = (float)M_PI / (float)kSubdivision;			// 緯度分割1つ分の角度

		// ~ 変数 ~ //
		Math::Vector3 center;	// 中心座標
		float radius;			// 半径
		Surface* surfaces[16][16];	// 描画する面 [kSubdivision][kSubdivision]

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(FillMode fillmode, Resource::Texture* texture = nullptr);
		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const { return kSubdivision * kSubdivision * 6; }
	};
}