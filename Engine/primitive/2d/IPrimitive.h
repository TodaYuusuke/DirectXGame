#pragma once
#include "../../math/Vector3.h"
#include "../../utility/Color.h"
#include <wrl.h>
#include <d3d12.h>

namespace LWP::Primitive {
	// インスタンスをコントローラに格納させるため、コンストラクタでポインタを受け取る
	class Manager;
	enum FillMode : int;

	struct Vertex {
		Math::Vector3 position;	// 座標
		Utility::Color color;	// 色
	};

	class IPrimitive {
	public:
		//*** 純粋仮想関数 ***//

		// 描画
		virtual void Draw(FillMode fillmode) = 0;
		// 頂点数を返す関数
		virtual int GetVertexCount() const = 0;

	protected:
		Manager* primitiveManager;
	};
}