#pragma once
#include "../GPUDevice.h"

#include "utility/IndexManager.h"
#include "utility/Counter.h"

namespace LWP::Base {
	// Heap情報構造体
	struct HeapInfo {
		Utility::Index index;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuView;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuView;

		void SetView(IDescriptorHeap* heap) {
			cpuView = heap->GetCPUHandle(index);
			gpuView = heap->GetGPUHandle(index);
		}
	};

	/// <summary>
	/// ディスクリプタヒープ基底クラス
	/// </summary>
	class IDescriptorHeap {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IDescriptorHeap() = delete;
		/// <summary>
		/// コンストラクタ（IndexManagerの初期化値要求）
		/// </summary>
		IDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size);


		/// <summary>
		/// CPU上のDescriptorHandleを取得する関数
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);
		/// <summary>
		/// GPU上のDescriptorHandleを取得する関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);


	protected: // ** メンバ変数 ** //
		// Heap
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap_;
		// ディスクリプタ1つ分のサイズ定数
		const uint32_t kElementSize;
		// ディスクリプタヒープの最大サイズ
		const uint32_t kMaxSize;

		// インデックス管理
		Utility::IndexManager indexManager_;


	public: // ** getter ** //
		// Heapを受け取る関数
		ID3D12DescriptorHeap* GetHeap() const { return heap_.Get(); }
		// ディスクリプタ1つ分のサイズを受け取る関数
		uint32_t GetDescriptorSize() const { return kDescriptorSize_; }

		/// <summary>
		/// 使用数を+1増加
		/// </summary>
		int GetAndIncrement() { return count_.GetAndIncrement(); }
		/// <summary>
		/// 現在の使用数を取得
		/// </summary>
		int GetCount() { return count_.Get(); }

	protected: // ** 外部変数 ** //
		// デバイスのポインタ
		ID3D12Device* device_ = nullptr;

		// 使用済みカウンタ
		LWP::Utility::Counter count_;


	protected: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// ディスクリプタヒープを生成する関数
		/// </summary>
		ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

		/// <summary>
		///　デスクリプタヒープのデスクリプタハンドル増分サイズを返す関数
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandleIncrementSize(const D3D12_CPU_DESCRIPTOR_HANDLE& other, int offsetInDescriptors, UINT descriptorIncrementSize) const;

	};
}