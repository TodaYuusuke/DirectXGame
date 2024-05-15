#pragma once
#include "base/directX/utility/RootSignature.h"
#include "base/directX/utility/Command.h"
#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/resource/rendering/RenderResource.h"

#include <string>
#include <fstream>

namespace LWP::Base::PostProcess {
	/// <summary>
	/// ポストプロセス達の基底クラス
	/// </summary>
	class IPostProcess {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		IPostProcess() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IPostProcess() = default;

		// 初期化
		virtual void Init() = 0;
		// 更新
		virtual void Update() = 0;

		// 必要な構造体、バインド情報、関数を書き込む
		virtual void WriteBinding(std::ofstream* stream, RootSignature* root, int* i) { stream; root; i; }
		// main内の処理を書き込む
		virtual void WriteProcess(std::ofstream* stream) = 0;
		// commandListにBind情報を指示
		virtual void BindCommand(ID3D12GraphicsCommandList* list, int* offset) { list; offset; }
		// 事前に行わなければいけない処理をここにまとめる
		virtual void PreCommand(ID3D12GraphicsCommandList* list, Base::RenderResource* target) { list; target; }

		/// <summary>
		/// ImGui
		/// </summary>
		virtual void DebugGUI() = 0;
		
	public: // ** 共通のメンバ変数 ** //

		// 使用するかフラグ
		bool use = false;
		// 自分のbind番号
		int bindIndex = -1;
	};
}