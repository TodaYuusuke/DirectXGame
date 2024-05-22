#include "HeapManager.h"

#include "base/ImGuiManager.h"

using namespace LWP::Base;

HeapManager::HeapManager(GPUDevice* gpuDevice) {
	// RTV作成
	rtv_ = std::make_unique<RTV>(gpuDevice->GetDevice());
	rtv_->Init();
	// SRV作成
	srv_ = std::make_unique<SRV>(gpuDevice->GetDevice());
	srv_->Init();
	// DSV作成
	dsv_ = std::make_unique<DSV>(gpuDevice->GetDevice());
	dsv_->Init();
}

void HeapManager::DebugGUI() {
#if DEMO
	ImGui::Begin("LWP", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginTabBar("LWP")) {
		if (ImGui::BeginTabItem("Base")) {

			rtv_->DebugGUI();
			srv_->DebugGUI();
			dsv_->DebugGUI();

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
#endif
}
