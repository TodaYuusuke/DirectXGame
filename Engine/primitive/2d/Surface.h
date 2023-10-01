#pragma once
#include "../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// 平面
	/// </summary>
	class Surface final
		: public IPrimitive {
	public:
		// ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		using IPrimitive::IPrimitive;

		/// <summary>
		/// 頂点を生成する関数
		/// </summary>
		void CreateVertices() override;

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const { return 4; }
	};
}