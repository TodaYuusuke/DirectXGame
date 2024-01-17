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

		// レンダリング解像度
		const int kWidth;
		const int kHeight;

		// コンストラクタ
		RenderTexture() = delete;
		RenderTexture(Base::CommandManager* manager, const int width, const int height);
		// デストラクタ
		~RenderTexture() = default;

		// レンダリング解像度を変更する関数
		//void ChangeRenderResolution(const int width, const int height);

		// サイズをVector2で受け取る
		Math::Vector2 GetTextureSize() const override;
		
		// リソースを返す関数
		ID3D12Resource* GetResource() { return resource_.Get(); }
		// RTVのインデックスを返す関数
		int GetRTV() { return rtvIndex_; }
		// DSVのインデックス
		int GetDSV() { return dsvIndex_; }

	private: // ** メンバ変数 ** //

		// RTVのインデックス
		int rtvIndex_ = -1;
		// DSVのインデックス
		int dsvIndex_ = -1;
	};
}