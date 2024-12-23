#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

#include "../../Externals/imgui/imgui.h"
#include "../../Externals/imgui/imgui_impl_dx12.h"
#include "../../Externals/imgui/imgui_impl_win32.h"
#include "../../Externals/imgui/imgui_stdlib.h"

#include "../utility/Color.h"

#include "base/directX/utility/descriptorHeap/SRV.h"

namespace LWP::Base {

	class ImGuiManager {
	public:
		ImGuiManager() = default;
		~ImGuiManager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

		/// <summary>
		/// ImGui受付開始
		/// </summary>
		void Begin();


	private:
		// DirectX基盤インスタンス（借りてくる）
		DirectXCommon* dxCommon_ = nullptr;
		// SRV上の登録情報
		SRVInfo srvInfo;

		ImGuiManager(const ImGuiManager&) = delete;
		const ImGuiManager& operator=(const ImGuiManager&) = delete;

		// ImGuiをLWP用に改善した関数たち
	public:
		/// <summary>
		/// ColorPicker4 ... Colorクラス対応ver
		/// </summary>
		/// <param name="label"></param>
		/// <param name="col">/param>
		/// <param name="flags"></param>
		/// <param name="ref_col"></param>
		static void ColorPicker4(const char* label, Utility::Color& col, ImGuiColorEditFlags flags = 0, const float* ref_col = (const float*)0);

		/// <summary>
		/// ColorEdit3 ... Colorクラス対応ver
		/// </summary>
		/// <param name="label"></param>
		/// <param name="col"></param>
		/// <param name="flags"></param>
		static void ColorEdit4(const char* label, Utility::Color& col, ImGuiColorEditFlags flags = 0);
	};
}