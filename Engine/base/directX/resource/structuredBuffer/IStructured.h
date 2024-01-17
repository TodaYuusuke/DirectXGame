#pragma once
#include "../IResource.h"
#include "../../descriptorHeap/SRV.h"

namespace LWP::Base {

	/// <summary>
	/// StructuredBuffer用の汎用リソースクラス
	/// </summary>
	template<typename T>
	class IStructured : public IResource {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		IStructured(ID3D12Device* device, SRV* srv, UINT maxSize) : kMax_(maxSize) {
			D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			desc.Buffer.NumElements = maxSize;
			desc.Buffer.StructureByteStride = sizeof(T);

			resource_ = CreateBufferResource(device, sizeof(T) * maxSize);
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			view_ = srv->GetGPUHandle(srv->GetCount());
			device->CreateShaderResourceView(resource_.Get(), &desc, srv->GetCPUHandle(srv->GetAndIncrement()));
		};
		virtual ~IStructured() = default;

		// 最後尾にデータを追加（返り値は追加した場所のインデックス）
		int AddData(const T& value) {
			// 最大数を超えていないかチェック
			assert(GetCount() < static_cast<int>(kMax_));
			data_[GetCount()] = value;
			return usedCount_.GetAndIncrement();
		}

		// Viewを返す関数
		D3D12_GPU_DESCRIPTOR_HANDLE GetView() { return view_; }
		// Viewを外部からセットする関数（subRendering用）
		void SetView(D3D12_GPU_DESCRIPTOR_HANDLE view) { view_ = view; }


	protected: // ** メンバ変数 ** //

		// 最大サイズ
		const UINT kMax_;
		// データ
		T* data_ = nullptr;

		// リソースへのView
		D3D12_GPU_DESCRIPTOR_HANDLE view_;

	private: // ** プライベートな関数 ** //

		ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t size) {
			HRESULT hr = S_FALSE;

			ID3D12Resource* resource;

			// 頂点リソース用のヒープの設定
			D3D12_HEAP_PROPERTIES uploadHeapProperties{};
			uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
			// 頂点リソースの設定
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
			// 実際に頂点リソースを作る
			hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
			assert(SUCCEEDED(hr));

			return resource;
		}
	};


	// 今だけグローバルで定義しておく
	static inline ID3D12Resource* CreateBufferResourceStatic(ID3D12Device* device, size_t size) {
		HRESULT hr = S_FALSE;

		ID3D12Resource* resource;

		// 頂点リソース用のヒープの設定
		D3D12_HEAP_PROPERTIES uploadHeapProperties{};
		uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;	// UploadHeapを使う
		// 頂点リソースの設定
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
		// 実際に頂点リソースを作る
		hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
		assert(SUCCEEDED(hr));

		return resource;
	}
}