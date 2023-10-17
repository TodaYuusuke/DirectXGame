#pragma once
#include "../GPUDevice.h"

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
		// ディスクリプタのサイズ定数
		uint32_t kDescriptorSize_;

		// ディスクリプタのサイズ
		uint32_t size_;


	public: // ** getter ** //
		// Heapを受け取る関数
		ID3D12DescriptorHeap* GetHeap() const { return heap_.Get(); }
		// ディスクリプタ1つ分のサイズを受け取る関数
		uint32_t GetDescriptorSize() const { return kDescriptorSize_; }


	protected: // ** 外部変数 ** //
		// デバイスのポインタ
		ID3D12Device* device_ = nullptr;


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