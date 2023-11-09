#include "PointLight.h"

#include <base/ImGuiManager.h>
#include <base/directX/command/CommandManager.h>

using namespace LWP::Math;
using namespace LWP::Object;

// 初期化
void PointLight::Initialize() {
	transform.Initialize();
}
// 更新
void PointLight::Update(Base::CommandManager* manager) {
	if (!isActive) { return; }
		
	// 6方向のビュープロジェクション
	Vector3 rotation[6] = {
		{0.0f,1.57f,0.0f},
		{0.0f,-1.57f,0.0f},
		{-1.57f,0.0f,0.0f},
		{1.57f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,3.14f,0.0f},// 多分ここだけ違う
	};

	Matrix4x4 viewProjections[6];
	for (int i = 0; i < 6; i++) {
		//Matrix4x4 viewMatrix = transform.GetWorldMatrix().Inverse();
		Matrix4x4 viewMatrix = (Matrix4x4::CreateRotateXYZMatrix(rotation[i]) * Matrix4x4::CreateTranslateMatrix(transform.translation)).Inverse();
		Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(1.571f, 1.0f, 0.1f, 100.0f);
		viewProjections[i] = viewMatrix * projectionMatrix;
	}

	// CommandManagerにデータを登録
	manager->SetPointLightData(this, viewProjections);
}

// デバッグ用GUI
void PointLight::DebugGUI() {
	transform.DebugGUI();
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::DragFloat("Radius", &radius, 0.01f);
	ImGui::DragFloat("Decay", &decay, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
}