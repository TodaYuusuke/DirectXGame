#pragma once
#include "ITexture.h"

namespace LWP::Base {
	class CommandManager;
}

namespace LWP::Resource {
	/// <summary>
	/// レンダリングされた結果のテクスチャ
	/// </summary>
	class RenderTexture final
		: public ITexture {
	public:
		// ** 共通の変数 ** //

		// コンストラクタ
		RenderTexture(Base::CommandManager* manager, const int width, const int height);
		// デストラクタ
		~RenderTexture() = default;

		// サイズをVector2で受け取る
		Math::Vector2 GetTextureSize() const override;
		
	private: // ** メンバ変数 ** //

		const int kWidth;
		const int kHeight;
	};
}