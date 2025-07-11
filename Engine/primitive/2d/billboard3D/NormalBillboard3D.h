#pragma once
#include "IBillboard3D.h"
#include "../sprite/NormalSprite.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常のビルボード3D
	/// </summary>
	class NormalBillboard3D final
		: public IBillboard3D, NormalSprite {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		NormalBillboard3D() { name = "NormalBillboard3D" + name; }

		// 名前を返す関数（ISpriteのnameにアクセスする用）
		std::string& GetName() override { return name; }
	};
}