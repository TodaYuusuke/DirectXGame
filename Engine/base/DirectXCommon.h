#pragma once
#include "WinApp.h"

#include "directX/utility/GPUDevice.h"
#include "directX/utility/descriptorHeap/RTV.h"
#include "directX/utility/descriptorHeap/SRV.h"
#include "directX/utility/descriptorHeap/DSV.h"
#include "directX/renderPass/RenderingPipeline.h"

#include <memory>


namespace LWP::Object {
	class Camera;
}

namespace LWP::Base {
	/// <summary>
	/// DirectX管理クラス
	/// </summary>
	class DirectXCommon : public Utility::ISingleton<DirectXCommon> {
		friend class Utility::ISingleton<DirectXCommon>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		DirectXCommon();

	public:	// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();

		/// <summary>
		/// DrawCall
		/// </summary>
		void DrawCall();

		/// <summary>
		/// Debug用GUI
		/// </summary>
		void DebugGUI();

		// ImGui用
		UINT GetBufferCount() { return renderingPipeline_->GetBufferCount(); }
		DXGI_FORMAT GetFormat() { return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; }


	private: // ** メンバ変数 ** //

		// レンダリングパイプライン
		std::unique_ptr<RenderingPipeline> renderingPipeline_;


	private: // 非公開のメンバ関数
		DirectXCommon(const DirectXCommon&) = delete;
		const DirectXCommon& operator=(const DirectXCommon&) = delete;
	};
}