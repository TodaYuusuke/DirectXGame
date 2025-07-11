#pragma once
#include "IBillboard3D.h"
#include "../sprite/ClipSprite.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常のビルボード3D（切り抜きテクスチャver）
	/// </summary>
	class ClipBillboard3D final
		: public ClipSprite, IBillboard3D {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ClipBillboard3D() { name = "ClipBillboard3D" + name; }

		// 名前を返す関数（ISpriteのnameにアクセスする用）
		std::string& GetName() override { return name; }
	};
}