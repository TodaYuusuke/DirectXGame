#pragma once
#include <math/vector/Vector2.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

namespace LWP::Resource {
	class ITexture {
	protected: // ** プロパティ変数 ** //
		// CommandManager上でのインデックス
		int index_;
	public: // アクセッサ
		// CommandManager上でのインデックスを読みとる
		int GetIndex() const { return index_; }
		// 別のMaterialと同期する
		void SetIndex(int value) { index_ = value; }


	public: // ** メンバ関数 ** //
		// サイズをVector2で受け取る
		virtual Math::Vector2 GetTextureSize() const = 0;

	private: // ** メンバ変数 ** //

		// リソース
		ID3D12Resource* resource_ = nullptr;
	};
}