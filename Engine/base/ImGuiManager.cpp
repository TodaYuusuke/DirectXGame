#include "ImGuiManager.h"
#include "../math/vector/Vector4.h"
#include <Adapter.h>

#include "directX/utility/descriptorHeap/SRV.h"

using namespace LWP::Base;
using namespace LWP::Utility;
using namespace LWP::Math;

void ImGuiManager::Init() {
	dxCommon_ = DirectXCommon::GetInstance();

	// ImGuiの初期化、詳細はさして重要ではないので省略
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	// 拡大率を適応
	int factor = WinApp::GetInstance()->GetScaleFactor();
	if (factor != -1) {
		ImGui::GetStyle().ScaleAllSizes(static_cast<float>(factor / 100.0f));
	}
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());
	// SRV上に登録してもらう
	srvInfo = SRV::GetInstance()->CreateImGuiSpace();
	ImGui_ImplDX12_Init(GPUDevice::GetInstance()->GetDevice(),
		dxCommon_->GetBufferCount(),
		dxCommon_->GetFormat(),
		SRV::GetInstance()->GetHeap(),
		srvInfo.cpuView,
		srvInfo.gpuView
	);
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