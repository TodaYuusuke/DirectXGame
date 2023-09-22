#pragma once
#include "../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// 平面
	/// </summary>
	class Surface final
		: public IPrimitive {
	public:
		// ** 変数 ** //

		/// 頂点（※必ず右回りに座標を配置すること）
		Vertex vertices[4]{};


		// ** 関数 ** //
		
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Surface(Manager*);

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(FillMode fillmode, Resource::Texture* texture = nullptr);
		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const { return 4; }
	};
}