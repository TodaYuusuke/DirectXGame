#pragma once
#include "../IRenderPass.h"

#include "../../resource/rendering/RenderResource.h"

namespace LWP::Base {
	/// <summary>
	/// リソース同士のコピーをする
	/// </summary>
	class Copy final
		: public IRenderPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		Copy() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Copy() override = default;

		/// <summary>
		/// コピー命令を積み込む
		/// </summary>
		/// <param name="src">コピー元のリソース</param>
		/// <param name="dst">コピー先のリソース</param>
		void PushCommand(IRenderingResource* src, IRenderingResource* dst, ID3D12GraphicsCommandList6* list);
	};
}