#pragma once
#include "IGraphicsPass.h"

namespace LWP::Base {
	class BackBuffer;
}

namespace LWP::Base {
	/// <summary>
	/// 最後にImGuiをレンダリングする
	/// </summary>
	class ImGuiRender final
		: public IGraphicsPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		ImGuiRender() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~ImGuiRender() override = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 描画命令を積み込む
		/// </summary>
		void PushCommand(BackBuffer* backBuffer, ID3D12GraphicsCommandList6* list);
	};
}