#pragma once
#include "math/vector/Vector2.h"
#include "math/vector/Vector4.h"
#include "primitive/2d/Sprite.h"
#include "resources/texture/Texture.h"

namespace LWP::Utility {
	/// <summary>
	/// UI管理クラス
	/// </summary>
	class UI final {
	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		UI& AddSprite(LWP::Resource::Texture texture, LWP::Math::Vector2 position, LWP::Math::Vector2 anchor);

		// 全部のisActiveを変更する関数
		void SetIsActive(bool b);

		// 配列
		std::vector<LWP::Primitive::Sprite> sprites_;
	};
}