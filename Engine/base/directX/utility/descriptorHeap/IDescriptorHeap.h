#pragma once
#include "../GPUDevice.h"

#include "utility/IndexManager.h"
#include <utility>

namespace LWP::Base {
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
		IDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size);
		/// <summary>
		/// コンストラクタ（SRV用の特殊なコンストラクタ）
		/// </summary>
		IDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t DescSize, uint32_t indexSize);


		/// <summary>
		/// CPU上のDescriptorHandleを取得する関数
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index);
		/// <summary>
		/// GPU上のDescriptorHandleを取得する関数
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index);

		/// <summary>
		/// ImGui
		/// </summary>
		virtual void DebugGUI() = 0;


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
		uint32_t GetDescriptorSize() const { return kElementSize; }


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


	// Heap情報構造体
	struct HeapInfo {
	public:
		Utility::Index index;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuView{};

		// デフォルトコンストラクタ
		HeapInfo() = default;
		~HeapInfo() = default;

		virtual void SetView(IDescriptorHeap* heap) {
			cpuView = heap->GetCPUHandle(index);
		}

		// ムーブコンストラクタ
		HeapInfo(HeapInfo&& other) noexcept {
			index = std::exchange(other.index, Utility::Index());
			cpuView = std::exchange(other.cpuView, {});
		}

		// ムーブ代入演算子
		HeapInfo& operator=(HeapInfo&& other) noexcept {
			if (this != &other) {
				// ムーブ代入の実装
				index = std::exchange(other.index, Utility::Index());
				cpuView = std::exchange(other.cpuView, {});
			}
			return *this;
		}

		// コピー操作を禁止
		HeapInfo(const HeapInfo&) = delete;
		HeapInfo& operator=(const HeapInfo&) = delete;
	};
}