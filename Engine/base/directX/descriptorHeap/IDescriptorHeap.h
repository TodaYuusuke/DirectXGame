#pragma once
#include "../GPUDevice.h"

#include "utility/Counter.h"

namespace LWP::Base {
	/// <summary>
	/// ディスクリプタヒープ
	/// </summary>
	class IDescriptorHeap {
	public:
		// ** メンバ関数 ** //

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
		uint32_t kDescriptorSize_;

		// ディスクリプタヒープのサイズ
		uint32_t size_;


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