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

	// ImNodeの初期化
	ImNodes::CreateContext();
}
void ImGuiManager::Release() {
	// ImNodeの解放
	ImNodes::DestroyContext();
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

void ImGuiManager::InputText(const char* label, std::string& str, const int maxSize, ImGuiInputTextFlags flags) {
	const int bufferSize = maxSize + 1; // 終端文字分の1バイトを追加
	char* c = new char[bufferSize];
	strncpy_s(c, bufferSize, str.c_str(), _TRUNCATE);	// char型に変換
	ImGui::InputText(label, c, bufferSize, flags);
	str = c;	// 文字列を更新
	delete[] c;	// char型のメモリを解放
}
void ImGuiManager::ShowTexture(const Resource::Texture& texture, float scale) {
	if (texture.GetIndex() != -1) {	// テクスチャが存在するならば表示
		Math::Vector2 size = texture.GetSize() * scale;
		ImGui::Image((ImTextureID)SRV::GetInstance()->GetGPUHandle(texture.GetIndex()).ptr, ImVec2(size.x, size.y));
	}
	else {
		ImGui::Text("No exist");	// 存在しないので非表示
	}
}
void ImGuiManager::ShowRenderResource(const RenderResource& resource, float scale) {
	if (resource.srvInfo.index != -1) {	// テクスチャが存在するならば表示
		Math::Vector2 size = resource.GetTextureSize() * scale;
		ImGui::Image((ImTextureID)resource.srvInfo.gpuView.ptr, ImVec2(size.x, size.y));
	}
	else {
		ImGui::Text("No exist");	// 存在しないので非表示
	}
}
void ImGuiManager::ShowShadowMap(const SM_Direction& sm) {
	if (sm.srvInfo.index != -1) {	// テクスチャが存在するならば表示
		Math::Vector2 size = Vector2{ static_cast<float>(sm.width), static_cast<float>(sm.height) };
		size *= (512.0f / size.x);
		ImGui::Image((ImTextureID)SRV::GetInstance()->GetGPUHandle(sm.srvInfo.index).ptr, ImVec2(size.x, size.y));
	}
	else {
		ImGui::Text("No exist");	// 存在しないので非表示
	}
}