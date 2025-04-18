#pragma once
#include "utility/Singleton.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl.h>

namespace LWP::Base {
	/// <summary>
	/// グラボ関連のクラス
	/// </summary>
	class GPUDevice final : public Utility::ISingleton<GPUDevice> {
		friend class Utility::ISingleton<GPUDevice>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		GPUDevice();


	private: // ** プロパティ変数 ** //
		// DXGIファクトリー
		Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	public:
		// DXGIファクトリーを受け取る関数
		IDXGIFactory7* GetFactory() const { return dxgiFactory_.Get(); }

	private:
		// DXGIデバイス
		Microsoft::WRL::ComPtr<ID3D12Device2> device_;
	public:
		// DXGIデバイス受け取る関数
		ID3D12Device2* GetDevice() const { return device_.Get(); }
	};
}