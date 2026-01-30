#pragma once
#include "IPostProcessPass.h"

namespace LWP::Base {
	/// <summary>
	/// 任意のカスタムシェーダーを使用したポストプロセスを行うためのパス
	/// </summary>
	class CustomShaderRender
		: public IPostProcessPass {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// デストラクタ
		/// </summary>
		CustomShaderRender() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~CustomShaderRender() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 描画命令を積み込む
		/// </summary>
		void PushCommand(ID3D12GraphicsCommandList6* list);
	};
}