#pragma once
#include "base/directX/utility/resource/IResource.h"

namespace LWP::Resource {
	/// <summary>
	/// リソースをテクスチャとして扱う基底クラス
	/// </summary>
	class ITexture : public LWP::Base::IResource {
	protected: // ** プロパティ変数 ** //
		// SRV上の登録データ
		LWP::Base::SRVInfo info_;
	public: // アクセッサ
		// SRV上のインデックス番号を返す
		int GetIndex() const { return info_.index; }


	public: // ** メンバ関数 ** //
		// サイズをVector2で受け取る
		virtual Math::Vector2 GetTextureSize() const = 0;
	};
}