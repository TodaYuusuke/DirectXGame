#pragma once
#include "ITexture.h"
#include "base/directX/resource/RenderResource.h"

// 前方宣言
namespace LWP::Base {
	class DirectXCommon;
}

namespace LWP::Resource {
	/// <summary>
	/// レンダリングされた結果のテクスチャ
	/// </summary>
	class RenderTexture final
		: public ITexture {
	public: // ** 共通の変数 ** //

		// レンダリング解像度
		const int kWidth;
		const int kHeight;

		// コンストラクタ
		RenderTexture() = delete;
		RenderTexture(Base::DirectXCommon* directX, const int width, const int height);
		// デストラクタ
		~RenderTexture() = default;

		// レンダリング解像度を変更する関数
		//void ChangeRenderResolution(const int width, const int height);

		// レンダーリソースを返す関数
		Base::RenderResource* GetRenderResource() { return renderResource_.get(); }
		// テクスチャ専用のリソースを返す関数
		ID3D12Resource* GetTexResource() { return texResource_.Get(); }

		// サイズをVector2で受け取る
		Math::Vector2 GetTextureSize() const override { return renderResource_->GetResolution(); };


	private: // ** メンバ変数 ** //

		// レンダリング結果をコピーし、テクスチャとしてのみ扱うリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> texResource_ = nullptr;

		// レンダリング先のテクスチャ
		std::unique_ptr<Base::RenderResource> renderResource_ = nullptr;
	};
}