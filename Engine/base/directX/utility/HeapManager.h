#pragma once
#include "descriptorHeap/SRV.h"	// なんか先にこっちをincludeしないとSRVInfoが正しく読み込まれない
#include "descriptorHeap/RTV.h"
#include "descriptorHeap/DSV.h"

#include <memory>


namespace LWP::Base {
	/// <summary>
	/// ディスクリプタヒープを管理するクラス
	/// </summary>
	class HeapManager {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		HeapManager() = delete;
		HeapManager(GPUDevice* gpuDevice);
		~HeapManager() = default;

		/// <summary>
		/// ImGui
		/// </summary>
		void DebugGUI();


	public: // ** ゲッター ** //

		RTV* rtv() { return rtv_.get(); }
		SRV* srv() { return srv_.get(); }
		DSV* dsv() { return dsv_.get(); }


	private: // ** メンバ変数 ** //
		
		// RTV
		std::unique_ptr<RTV> rtv_ = nullptr;
		// SRV
		std::unique_ptr<SRV> srv_ = nullptr;
		// DSV
		std::unique_ptr<DSV> dsv_ = nullptr;

		
	private: // ** 非公開のメンバ関数 ** //
		HeapManager(const HeapManager&) = delete;
		const HeapManager& operator=(const HeapManager&) = delete;
	};
}