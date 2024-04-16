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
		ConstantBuffer() : kElementSize(sizeof(T)) {}
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~ConstantBuffer() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="device"></param>
		void Init(GPUDevice* device) {
			// リソース作成
			CreateResource(device, kElementSize);
			// リソースをマップ
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			// viewを保持
			view_ = resource_->GetGPUVirtualAddress();
		}

		/// <summary>
		/// GPU上のViewを取得
		/// </summary>
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUView() const { return view_; }


	public: // ** パブリックなメンバ変数 ** //
		// 実際のデータ
		T* data_ = nullptr;

	private: // ** メンバ変数 ** //

		// データ1つ分のサイズ定数
		const uint32_t kElementSize;
		// リソースへのGPUView
		D3D12_GPU_VIRTUAL_ADDRESS view_;


	public: // ** オペレーターオーバーロード ** //

		// ID3D12Resource型への暗黙の変換演算子をオーバーロード
		operator ID3D12Resource* () {
			return resource_.Get();
		}
	};
}