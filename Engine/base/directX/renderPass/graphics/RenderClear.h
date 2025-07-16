#pragma once
#include "IGraphicsPass.h"

namespace LWP::Base {
	class IRenderingResource;
	class BackBuffer;
	class RenderResource;
	class DepthStencil;
}

namespace LWP::Base {
	/// <summary>
	/// リソースをクリアするパス
	/// </summary>
	class RenderClear final
		: public IGraphicsPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		RenderClear() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~RenderClear() override = default;

		/// <summary>
		/// BackBufferをクリアする命令を積み込む
		/// </summary>
		void PushCommand(BackBuffer* resource, ID3D12GraphicsCommandList6* list);
		/// <summary>
		/// RenderResourceをクリアする命令を積み込む
		/// </summary>
		void PushCommand(RenderResource* resource, ID3D12GraphicsCommandList6* list);
		/// <summary>
		/// DepthStencilをクリアする命令を積み込む
		/// </summary>
		void PushCommand(DepthStencil* resource, ID3D12GraphicsCommandList6* list);

		/// <summary>
		/// 全てのカメラをクリアする
		/// </summary>
		void ClearAllCamera(ID3D12GraphicsCommandList6* list);
	};
}