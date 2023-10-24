#pragma once
#include "ICommand.h"

namespace LWP::Base {
	/// <summary>
	/// コマンドクラス
	/// </summary>
	class MainCommand : public ICommand {
	public:
		// ** メンバ関数 ** //

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(ID3D12Device* device);

		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw(ID3D12RootSignature* rootSignature);

		/// <summary>
		/// 描画語処理
		/// </summary>
		void PostDraw();

		/// <summary>
		/// リセット用関数
		/// </summary>
		void Reset();
	};
}