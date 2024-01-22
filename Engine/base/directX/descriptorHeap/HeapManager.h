#pragma once
#include "RTV.h"
#include "SRV.h"
#include "DSV.h"

#include <memory>

namespace LWP::Base {
	/// <summary>
	/// ディスクリプタヒープを管理するクラス
	/// </summary>
	class HeapManager {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		HeapManager() = delete;
		HeapManager(HWND hwnd, GPUDevice* gpuDevice, int32_t width, int32_t height, ID3D12CommandQueue* queue);
		~HeapManager() = default;

	public: // ** ゲッター ** //

		RTV* rtv() { return rtv_.get(); }
		SRV* srv() { return srv_.get(); }
		DSV* dsv() { return dsv_.get(); }


	private: // ** メンバ変数 ** //
		
		// RTV
		std::unique_ptr<RTV> rtv_;
		// SRV
		std::unique_ptr<SRV> srv_;
		// DSV
		std::unique_ptr<DSV> dsv_;

		
	private: // ** 非公開のメンバ関数 ** //
		HeapManager(const HeapManager&) = delete;
		const HeapManager& operator=(const HeapManager&) = delete;
	};
}