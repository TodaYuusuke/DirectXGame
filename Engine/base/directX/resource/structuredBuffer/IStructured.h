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

		IStructured(ID3D12Device* device, SRV* srv, UINT maxSize) : kMax_(maxSize) {
			D3D12_SHADER_RESOURCE_VIEW_DESC commonDesc{};
			commonDesc.Format = DXGI_FORMAT_UNKNOWN;
			commonDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			commonDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			commonDesc.Buffer.FirstElement = 0;
			commonDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

			resource_ = CreateBufferResource(sizeof(T) * maxSize);
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			D3D12_SHADER_RESOURCE_VIEW_DESC vertexDesc = { commonDesc };
			vertexDesc.Buffer.NumElements = maxSize;
			vertexDesc.Buffer.StructureByteStride = sizeof(T);
			view_ = srv->GetGPUHandle(srv->GetCount());
			device->CreateShaderResourceView(resource_.Get(), &vertexDesc, srv->GetCPUHandle(srv->GetAndIncrement()));
		};
		virtual ~IStructured() = default;

	protected: // ** メンバ変数 ** //
		
		// 最大サイズ
		const UINT kMax_;


		// データ
		T* data_;
		// マックスサイズ

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
}