#include "GPUParticle.h"

#include "base/ImGuiManager.h"

using namespace LWP::Object;
using namespace LWP::Base;

GPUParticle::GPUParticle() : data_(1024) {
	Initialize();
}

void GPUParticle::Initialize() {
	emitterSphere_.Init();
	data_.Init();
	count_.Init();
}
void GPUParticle::Update(Base::RendererManager* manager) {
	if (!isActive) { return; }
	manager->AddParticle(this);
}

void GPUParticle::Add(int value) {
	emitterSphere_.data_->count = value;
	emitterSphere_.data_->emit = true;
}
void GPUParticle::Add(int value, LWP::Math::Vector3 position) {
	emitterSphere_.data_->position = position;
	Add(value);
}
// デバッグ用GUI
void GPUParticle::DebugGUI() {
	if (ImGui::TreeNode("Model")) {
		model.DebugGUI();
		ImGui::TreePop();
	}
	static int value = 8;
	ImGui::InputInt("Value", &value);
	if (ImGui::Button("Particle Generate")) { Add(value); }
	ImGui::Checkbox("isActive", &isActive);
	//ImGui::Text("Particle Count %d", data_.size());
}

void GPUParticle::SetShaderPath(std::string emitter, std::string update) {
	root_.AddCBVParameter(0,  SV_All)
		.AddCBVParameter(1, SV_All)
		.AddUAVParameter(0, SV_All)	// 書き込むリソース
		.Build();
	emitterPSO_.Init(root_, PSO::Type::Compute)
		.SetCS(kDirectoryPath + emitter)
		.Build();
	updatePSO_.Init(root_, PSO::Type::Compute)
		.SetCS(kDirectoryPath + update)
		.Build();
}
