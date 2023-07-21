#pragma once
#include "IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// 三角形
	/// </summary>
	class Triangle final
		: public IPrimitive {
	public:
		// コンストラクタ
		Triangle(Manager*);

		/// 頂点（※必ず右回りに座標を配置すること）
		Vertex vertices[3]{};

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(FillMode fillmode);
		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const { return 3; }
	};
}