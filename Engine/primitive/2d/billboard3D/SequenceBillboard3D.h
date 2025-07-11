#pragma once
#include "IBillboard3D.h"
#include "../sprite/SequenceSprite.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常のビルボード3D（連番テクスチャver）
	/// </summary>
	class SequenceBillboard3D final
		: public SequenceSprite, IBillboard3D {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SequenceBillboard3D() { name = "SequenceBillboard3D" + name; }

		// 名前を返す関数（ISpriteのnameにアクセスする用）
		std::string& GetName() override { return name; }
	};
}