#include "GPUParticle.h"

#include "base/ImGuiManager.h"

using namespace LWP::Object;
using namespace LWP::Base;

GPUParticle::GPUParticle() : GPUParticle(multiply) {}
GPUParticle::GPUParticle(int capacityMultiply) : data_(1024 * capacityMultiply), freeListIndex_(1), freeList_(1024 * capacityMultiply), hitList_(1024 * capacityMultiply){
	Initialize();
	multiply = capacityMultiply;
	*count_.data_ = 1024 * multiply;
}

void GPUParticle::Initialize() {
	emitterSphere_.Init();
	data_.Init();
	freeListIndex_.Init();
	freeList_.Init();
	count_.Init();
	hitList_.Init();

	model.isActive = false;
}
void GPUParticle::Update(Base::RendererManager* manager) {
	if (!isActive) { return; }
	manager->AddParticle(this);
}

void GPUParticle::Add(uint32_t value) {
	emitCount = value;
	isEmit = true;
}
void GPUParticle::Add(uint32_t value, const LWP::Math::Vector3& position) {
	SetPosition(position);
	Add(value);
}
void GPUParticle::SetPosition(const LWP::Math::Vector3& position) {
	emitterSphere_.data_->position = position;
}
// デバッグ用GUI
void GPUParticle::DebugGUI() {
	if (ImGui::TreeNode("Model")) {
		model.DebugGUI();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Emitter")) {
		ImGui::DragFloat3("Position", &emitterSphere_.data_->position.x, 0.01f);
		ImGui::DragFloat("Radius", &emitterSphere_.data_->radius, 0.01f);
		ImGui::DragFloat("Frequency", &emitterSphere_.data_->frequency, 0.01f);
		ImGui::DragFloat("FrequencyTime", &emitterSphere_.data_->frequencyTime, 0.01f);
		ImGui::TreePop();
	}
	static int value = 8;
	ImGui::InputInt("Value", &value);
	if (ImGui::Button("Particle Generate")) { Add(value); }
	ImGui::Checkbox("isActive", &isActive);
	//ImGui::Text("Particle Count %d", data_.size());
}

void GPUParticle::SetShaderPath(std::string emitter, std::string update, std::string hit) {
	root_.AddCBVParameter(0,  SV_All)
		.AddCBVParameter(1, SV_All)
		.AddCBVParameter(2, SV_All)
		.AddUAVParameter(0, SV_All)	// 書き込むリソース
		.AddUAVParameter(1, SV_All)	// FreeListIndex
		.AddUAVParameter(2, SV_All)	// FreeList
		.AddTableParameter(0, SV_All)	// HitList
		.Build();
	emitterPSO_.Init(root_, PSO::Type::Compute)
		.SetCS(kDirectoryPath + emitter)
		.Build();
	updatePSO_.Init(root_, PSO::Type::Compute)
		.SetCS(kDirectoryPath + update)
		.Build();
	hitPSO_.Init(root_, PSO::Type::Compute)
		.SetCS(kDirectoryPath + hit)
		.Build();
}
