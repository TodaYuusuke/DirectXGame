#pragma once
#include "../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// 三角形
	/// </summary>
	class Triangle final
		: public IPrimitive {
	public:
		// ** 関数 ** //

		// 初期化を呼び出す
		Triangle() { Init(); }

		/// <summary>
		/// 頂点を生成する関数
		/// </summary>
		void CreateVertices() override;

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const { return 3; }
	};
}