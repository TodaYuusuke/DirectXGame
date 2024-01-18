#include "ImGuiManager.h"
#include "../math/vector/Vector4.h"
#include <Adapter.h>

using namespace LWP::Base;
using namespace LWP::Utility;
using namespace LWP::Math;

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {
	dxCommon_ = dxCommon;

	// ImGuiの初期化、詳細はさして重要ではないので省略
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().DisplaySize = { LWP::Info::GetWindowWidthF(),LWP::Info::GetWindowHeightF() };
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHWND());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
		dxCommon_->GetBufferCount(),
		dxCommon_->GetFormat(),
		dxCommon_->GetSRVHeap(),
		dxCommon_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart()
	);

}

void ImGuiManager::Finalize() {

}

void ImGuiManager::Begin() {
	// ImGui
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::ColorPicker4(const char* label, Utility::Color& col, ImGuiColorEditFlags flags, const float* ref_col) {
	Vector4 v{ col.GetVector4() };
	ImGui::ColorPicker4(label, &v.x, flags, ref_col);
	col = *new Color(v);
}

void ImGuiManager::ColorEdit4(const char* label, Utility::Color& col, ImGuiColorEditFlags flags) {
	Vector4 v{ col.GetVector4() };
	ImGui::ColorEdit4(label, &v.x, flags);
	col = *new Color(v);
}

//template <typename T>
//void ImGuiManager::CheckBox(const char* label, T* v) {
//	bool b = static_cast<bool>(v);
//	ImGui::Checkbox(label, &b);
//	*v = static_cast<T>(b);
//}