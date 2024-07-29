#pragma once
#include "../IResource.h"

namespace LWP::Base {
	/// <summary>
	/// シェーダー用のリソース基底クラス
	/// </summary>
	class IDataResource
		: public IResource {
	public: // ** パブリックなメンバ関数 ** //

		IDataResource() = default;
		virtual ~IDataResource() = default;

	protected: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// リソース作成
		/// </summary>
		/// <param name="device"></param>
		/// <param name="size"></param>
		void CreateResource(GPUDevice* device, size_t size) {
			HRESULT hr = S_FALSE;

			// ヒープの設定
			D3D12_HEAP_PROPERTIES uploadHeapProperties{};
			uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
			// 設定
			D3D12_RESOURCE_DESC resourceDesc{};
			// バッファリソース。テクスチャの場合はまた別の設定をする
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resourceDesc.Width = size; // リソースのサイズ
			// バッファの場合はこれらは1にする決まり
			resourceDesc.Height = 1;
			resourceDesc.DepthOrArraySize = 1;
			resourceDesc.MipLevels = 1;
			resourceDesc.SampleDesc.Count = 1;
			// バッファの場合はこれにする決まり
			resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			// 実際にリソースを作る
			hr = device->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource_));
			assert(SUCCEEDED(hr));
		}

	protected: // ** メンバ定数 ** //
		// データ1つ分のサイズ定数
		//const uint32_t kElementSize;

	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}