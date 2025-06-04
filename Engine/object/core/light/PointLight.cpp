#include "PointLight.h"

#include "component/System.h"
#include "base/ImGuiManager.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;

PointLight::PointLight() {
	// リソースの初期化
	for (int i = 0; i < 6; i++) {
		viewBuffers_[i].Init();
	}
	shadowMap_.Init();
}

// 初期化
void PointLight::Initialize() {
	name = "PointLight";
}
// 更新
void PointLight::Update(Base::RendererManager* manager) {
	if (!isActive) { return; }
		
	// 6方向のビュープロジェクション
	static Vector3 rotation[6] = {
		{0.0f,1.57f,0.0f},
		{0.0f,-1.57f,0.0f},
		{-1.57f,0.0f,0.0f},
		{1.57f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,-3.14f,0.0f},
	};

	for (int i = 0; i < 6; i++) {
		Vector3 worldPosition = worldTF.GetWorldPosition();
		Matrix4x4 viewMatrix = (Matrix4x4::CreateRotateXYZMatrix(rotation[i]) * Matrix4x4::CreateTranslateMatrix(worldPosition)).Inverse();
		Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(1.571f, 1.0f, 0.01f, 300.0f);
		*viewBuffers_[i].data_ = viewMatrix * projectionMatrix;
	}

	// CommandManagerにデータを登録
	manager->AddLightData(this);
	manager->AddTarget({
		viewBuffers_[0].GetGPUView(),
		viewBuffers_[1].GetGPUView(),
		viewBuffers_[2].GetGPUView(),
		viewBuffers_[3].GetGPUView(),
		viewBuffers_[4].GetGPUView(),
		viewBuffers_[5].GetGPUView()
		}, &shadowMap_);
}

// デバッグ用GUI
void PointLight::DebugGUI() {
	worldTF.DebugGUI();
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::DragFloat("Radius", &radius, 0.01f);
	ImGui::DragFloat("Decay", &decay, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
}