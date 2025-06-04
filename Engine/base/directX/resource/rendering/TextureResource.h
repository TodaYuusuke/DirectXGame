#pragma once
#include "../IResource.h"
#include "math/vector/Vector2.h"
#include "../Externals/DirectXTex/DirectXTex.h"

#include <string>

namespace LWP::Resource {
	class Texture;
}

namespace LWP::Base {
	/// <summary>
	/// リソースをテクスチャとして扱う基底クラス
	/// </summary>
	class TextureResource : public IResource {
	public: // ** メンバ関数 ** //

		// 初期化
		void Init(std::string filepath);

		// SRV上の登録インデックス番号を返す
		int GetSRVIndex() const { return info_.index; }
		// 解像度を返す関数
		Math::Vector2 GetTextureSize() const { return { static_cast<float>(width), static_cast<float>(height) }; }


	public: // ** パブリックなメンバ変数 ** //

		// 解像度
		int width;
		int height;

		// Resourceの設定
		D3D12_RESOURCE_DESC desc;
		// heapの設定
		D3D12_HEAP_PROPERTIES properties;


	private: // ** メンバ変数 ** //

		// ミップマップ付きテクスチャ
		DirectX::ScratchImage mipImages_;
		// SRV上の登録データ
		SRVInfo info_;

		// VRAM配置用の一時リソース
		ID3D12Resource* intermediateResource_;

	private: // ** プライベートなメンバ関数 ** //

		void Load(const std::string& filePath);

		// リソースを作成する関数
		ID3D12Resource* CreateResource(size_t size);

	public: // ** オペレーターオーバーロード ** //

		// Texture型への暗黙の変換演算子をオーバーロード
		//operator Resource::Texture();
	};
}