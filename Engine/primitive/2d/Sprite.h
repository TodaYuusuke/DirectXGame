#pragma once
#include "../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// スプライト
	/// </summary>
	class Sprite final
		: public IPrimitive {
	public:
		// コンストラクタ
		Sprite(Manager*);

		/// 頂点（※必ず右回りに座標を配置すること）
		Vertex3D vertices[4]{};

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