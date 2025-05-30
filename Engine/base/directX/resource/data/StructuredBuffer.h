#pragma once
#include "IDataResource.h"
#include "base/directX/utility/descriptorHeap/SRV.h"
#include "utility/Counter.h"

namespace LWP::Base {
	/// <summary>
	/// ストラクチャーバッファー用のリソースクラス
	/// </summary>
	template<class T>
	class StructuredBuffer 
		: public IDataResource {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ（定数を定義するため削除）
		/// </summary>
		StructuredBuffer() = delete;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		StructuredBuffer(uint32_t maxSize) : IDataResource(sizeof(T)), kMaxSize(maxSize) {}
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~StructuredBuffer() = default;

		void Init() {
			// リソース作成
			CreateResource(kElementSize_ * kMaxSize);
			// リソースをマップ
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

			// SRVへの登録設定
			D3D12_SHADER_RESOURCE_VIEW_DESC desc;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			desc.Buffer.FirstElement = 0;
			desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			desc.Buffer.NumElements = kMaxSize;
			desc.Buffer.StructureByteStride = kElementSize_;

			// SRVに登録
			info = SRV::GetInstance()->CreateStructuredBuffer(resource_.Get(), desc);
		}

		/// <summary>
		/// データを追加
		/// </summary>
		int Add(const T& t) {
			// 最大数を超えてないか確認
			assert(static_cast<uint32_t>(usedCount_.Get()) < kMaxSize);
			// データを追加しカウンターを進める
			data_[usedCount_.GetAndIncrement()] = t;

			// インデックスを返す
			return usedCount_.Get() - 1;
		}
		/// <summary>
		/// カウンターリセット
		/// </summary>
		void Reset() { usedCount_.Reset(); }
		/// <summary>
		/// 使用済みの数を返す
		/// </summary>
		/// <returns></returns>
		int GetCount() { return usedCount_.Get(); }

		/// <summary>
		/// GPU上のViewを取得
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUView() const { return info.gpuView; }

		/// <summary>
		/// SRVInfoを取得
		/// </summary>
		SRVInfo GetInfo() const { return info; }


	public: // ** メンバ定数 ** //

		// データ1つ分のサイズ定数
		//const uint32_t kElementSize;
		// データの最大数
		const uint32_t kMaxSize;

	public: // ** パブリックなメンバ変数 ** //

		// 実際のデータ
		T* data_ = nullptr;

	private: // ** メンバ変数 ** //
		// SRV上の登録データ
		SRVInfo info;

		// 使用済みカウント
		LWP::Utility::Counter usedCount_;

	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}