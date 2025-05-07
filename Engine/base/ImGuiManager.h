#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"

#include "../../Externals/imgui/imgui.h"
#include "../../Externals/imgui/imgui_impl_dx12.h"
#include "../../Externals/imgui/imgui_impl_win32.h"
#include "../../Externals/imgui/imgui_stdlib.h"

#include "../../Externals/imgui-node-editor/imgui_node_editor.h"

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
		void Init();

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
		/// ColorEdit4 ... Colorクラス対応ver
		/// </summary>
		/// <param name="label"></param>
		/// <param name="col"></param>
		/// <param name="flags"></param>
		static void ColorEdit4(const char* label, Utility::Color& col, ImGuiColorEditFlags flags = 0);
		
		/// <summary>
		/// 文字列の入出力を手伝う関数
		/// </summary>
		/// <param name="label"></param>
		/// <param name="str"></param>
		/// <param name="maxSize"></param>
		/// <param name="flags"></param>
		static void InputText(const char* label, std::string& str, const int maxSize = 256, ImGuiInputTextFlags flags = 0);

		/// <summary>
		/// リソースの中身を表示する
		/// </summary>
		/// <param name="index"></param>
		/// <param name="size"></param>
		static void ShowTexture(const Resource::Texture& texture, float scale = 1.0f);
		/// <summary>
		/// リソースの中身を表示する
		/// </summary>
		/// <param name="index"></param>
		/// <param name="size"></param>
		static void ShowRenderResource(const RenderResource& resource, float scale = 1.0f);
	};
}

// ノードエディターの別名定義
namespace ImGui::NE {
	using namespace ax::NodeEditor;
}
namespace ImGuiNE {
	using namespace ImGui::NE;
}