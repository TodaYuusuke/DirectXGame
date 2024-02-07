#pragma once
#include <Adapter.h>

namespace LWP::Utility {
	/// <summary>
	/// UI管理クラス
	/// </summary>
	class UI final {
	public: // **　メンバ関数 ** //

		/// <summary>
		/// ビルダーデザインパターン
		/// </summary>
		UI& AddSprite(LWP::Resource::Texture* texture, LWP::Math::Vector2 position, LWP::Math::Vector2 anchor);

		// 全部のisActiveを変更する関数
		void SetIsActive(bool b);


	private: // ** メンバ変数 ** //

		// 配列
		std::vector<LWP::Primitive::Sprite*> sprites_;
	};
};