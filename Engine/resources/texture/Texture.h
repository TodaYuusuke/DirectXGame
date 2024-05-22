#pragma once
#include "math/vector/Vector2.h"

// 前方宣言
namespace LWP::Base {
	// テクスチャリソース
	class TextureResource;
	// レンダーテクスチャ
	class RenderResource;
}

namespace LWP::Resource {
	/// <summary>
	/// テクスチャのインデックスを受け渡しする関数
	/// </summary>
	class Texture final {
	public: // ** メンバ関数 ** //
		// デフォルトコンストラクタ
		Texture() = default;
		// デストラクタ
		~Texture() = default;

		// サイズを取得する関数
		Math::Vector2 GetSize() const { return size_; }
		// インデックスを取得
		int GetIndex() const { return index_; }


	private: // ** メンバ変数 ** //

		// サイズ
		Math::Vector2 size_{};
		// SRV上のインデックス
		int index_ = -1;


	public: // ** オペレーターオーバーロード ** //

		// TextureResourceからの代入
		Texture(const Base::TextureResource& r);
		Texture& operator=(const Base::TextureResource& r);
		// RenderResourceからの代入
		Texture(const Base::RenderResource& r);
		Texture& operator=(const Base::RenderResource& r);

		// Observerクラス用
		bool operator==(Texture& other) {
			return index_ == other.index_;
		}
	};
}