#pragma once
#include "IDataResource.h"

namespace LWP::Base {
	/// <summary>
	/// コンスタントバッファー用のリソースクラス
	/// </summary>
	template<class T>
	class ConstantBuffer
		: public IDataResource {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ConstantBuffer() : IDataResource(sizeof(T)) {}
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~ConstantBuffer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="device"></param>
		void Init() {
			// リソース作成
			CreateResource(kElementSize_);
			// リソースをマップ
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}

		/// <summary>
		/// GPU上のViewを取得
		/// </summary>
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUView() const { return resource_->GetGPUVirtualAddress(); }


	public: // ** パブリックなメンバ変数 ** //
		// 実際のデータ
		T* data_ = nullptr;

	private: // ** メンバ変数 ** //

		// データ1つ分のサイズ定数
		//const uint32_t kElementSize{};
		// リソースへのGPUView
		//D3D12_GPU_VIRTUAL_ADDRESS view_{};


	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}