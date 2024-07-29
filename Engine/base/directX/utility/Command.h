#pragma once
#include "GPUDevice.h"

#include <cassert>
#include <vector>
#include <functional>

namespace LWP::Base {
	/// <summary>
	/// コマンド関連をまとめたクラス
	/// </summary>
	class Command final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Command() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Command() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Init(GPUDevice* device) {
			HRESULT hr = S_FALSE;

			// コマンドキューを生成する
			D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
			hr = device->GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&queue_));
			assert(SUCCEEDED(hr));	// コマンドキューの生成がうまくいかなかったので起動できない
			// コマンドアロケーターを生成する
			hr = device->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator_));
			assert(SUCCEEDED(hr));	// コマンドアロケーターの生成がうまくいかなかったので起動できない
			// コマンドリストを生成する
			hr = device->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator_.Get(), nullptr, IID_PPV_ARGS(&list_));
			assert(SUCCEEDED(hr));	// コマンドリストの生成がうまくいかなかったので起動できない

			// 初期値0でFenceを作る
			fenceVal_ = 0;
			hr = device->GetDevice()->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
			assert(SUCCEEDED(hr));
		};
		/// <summary>
		/// 描画命令実行
		/// </summary>
		void Execute() {
			HRESULT hr;
			// コマンドリストを閉じる
			list_->Close();

			// GPUにコマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { list_.Get() };
			queue_->ExecuteCommandLists(1, commandLists);

			// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
			queue_->Signal(fence_.Get(), ++fenceVal_);
			if (fence_->GetCompletedValue() != fenceVal_) {
				HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);
				assert(event != nullptr);
				fence_->SetEventOnCompletion(fenceVal_, event);
				WaitForSingleObject(event, INFINITE);
				CloseHandle(event);
			}

			// 次のコマンドリストを準備
			hr = allocator_->Reset();
			assert(SUCCEEDED(hr));
			hr = list_->Reset(allocator_.Get(), nullptr);
			assert(SUCCEEDED(hr));

			// 一時リソースを解放
			/*for (std::function<void()>& f : releaseFunctions_) {
				f();
			}*/
		}


		// dxcの汎用オブジェクト
		ID3D12CommandQueue* Queue() { return queue_.Get(); }
		// dxcのコンパイラオブジェクト
		ID3D12CommandAllocator* Allocator() { return allocator_.Get(); }
		// hlslファイル内でコンパイルするファイルの処理を行うハンドラ
		ID3D12GraphicsCommandList6* List() { return list_.Get(); }

		// 一時リソース解放用関数ポインタをセット
		//void SetReleaseFunction(std::function<void()> f) { releaseFunctions_.push_back(f); }

	private: // ** メンバ変数 ** //

		// コマンドキュー
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue_;
		// アロケーター
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator_;
		// リスト
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> list_;

		// GPU同期用のフェンス
		Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
		UINT64 fenceVal_ = 0;

		// テクスチャの一時リソースを解放する関数
		std::vector<int> releaseFunctions_;
	};
}