#pragma once
#include "IDataResource.h"
#include "base/directX/utility/descriptorHeap/SRV.h"
#include "utility/Counter.h"

namespace LWP::Base {
	/// <summary>
	/// ストラクチャーバッファー用のリソースクラス
	/// </summary>
	template<class T>
	class RWStructuredBuffer 
		: public IDataResource {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ（定数を定義するため削除）
		/// </summary>
		RWStructuredBuffer() = delete;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		RWStructuredBuffer(uint32_t maxSize) : IDataResource(sizeof(T)), kMaxSize(maxSize) {}
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~RWStructuredBuffer() = default;

		void Init() {
			// リソース作成(UAV用はフラグが特殊)
			HRESULT hr = S_FALSE;
			// ヒープの設定
			D3D12_HEAP_PROPERTIES uploadHeapProperties{};
			uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// UploadHeapを使う
			// 設定
			D3D12_RESOURCE_DESC resourceDesc{};
			resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resourceDesc.Width = kElementSize_ * kMaxSize; // リソースのサイズ
			resourceDesc.Height = 1;
			resourceDesc.DepthOrArraySize = 1;
			resourceDesc.MipLevels = 1;
			resourceDesc.SampleDesc.Count = 1;
			resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			// 実際にリソースを作る
			hr = GPUDevice::GetInstance()->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource_));
			assert(SUCCEEDED(hr));

			// UAVへの登録設定
			D3D12_UNORDERED_ACCESS_VIEW_DESC desc;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			//desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;	// GPUからアクセスできるように
			desc.Buffer.FirstElement = 0;
			desc.Buffer.CounterOffsetInBytes = 0;
			desc.Buffer.NumElements = kMaxSize;
			desc.Buffer.StructureByteStride = kElementSize_;

			// UAVに登録
			uavInfo = SRV::GetInstance()->CreateRWStructuredBuffer(resource_.Get(), desc);

			// 読み取り専用にSRVにも登録する

			// SRVへの登録設定
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			srvDesc.Buffer.NumElements = kMaxSize;
			srvDesc.Buffer.StructureByteStride = kElementSize_;

			// SRVに登録
			srvInfo = SRV::GetInstance()->CreateStructuredBuffer(resource_.Get(), srvDesc);
		}

		// バリアを設定


		/// <summary>
		/// リソースバリアを設定
		/// </summary>
		void SetResourceBarrier(ID3D12GraphicsCommandList* list) {
			// バリアを変えるわけではないが、依存関係を設定するためにバリアを張るだけの処理

			// TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			// 今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			// Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// 全てのサブリソースを選択
			barrier.Transition.Subresource = 0xFFFFFFFF;
			// バリアを張る対象のリソース
			barrier.Transition.pResource = resource_.Get();

			// リソースバリア変更
			list->ResourceBarrier(1, &barrier);
		}

		/// <summary>
		/// GPU上のViewを取得
		/// </summary>

		/// <summary>
		/// UAVInfoを取得
		/// </summary>
		UAVInfo GetUAVInfo() const { return uavInfo; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetUAVGPUView() const { return uavInfo.gpuView; }
		/// <summary>
		/// SRVInfoを取得
		/// </summary>
		SRVInfo GetSRVInfo() const { return srvInfo; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUView() const { return srvInfo.gpuView; }

		
	public: // ** メンバ定数 ** //

		// データ1つ分のサイズ定数
		//const uint32_t kElementSize;
		// データの最大数
		const uint32_t kMaxSize;


	private: // ** メンバ変数 ** //
		// UAV上の登録データ
		UAVInfo uavInfo;
		// SRV上の登録データ
		SRVInfo srvInfo;

	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}