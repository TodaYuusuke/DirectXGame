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
		// テクスチャ専用のリソースを返す関数
		ID3D12Resource* GetTexResource() { return texResource_.Get(); }
		// RTVのインデックスを返す関数
		int GetSRV() { return srvIndex_; }
		// RTVのインデックスを返す関数
		int GetRTV() { return rtvIndex_; }
		// DSVのインデックスを返す関数
		int GetDSV() { return dsvIndex_; }


	private: // ** メンバ変数 ** //

		// レンダリング結果をコピーし、テクスチャとしてのみ扱うリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> texResource_ = nullptr;

		// SRVのインデックス
		int srvIndex_ = -1;
		// RTVのインデックス
		int rtvIndex_ = -1;
		// 深度マップのリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> depthMapResource_ = nullptr;
		// DSVのインデックス
		int dsvIndex_ = -1;
	};
}