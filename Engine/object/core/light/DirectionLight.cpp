#include "DirectionLight.h"

#include <base/ImGuiManager.h>
#include <base/directX/command/CommandManager.h>

using namespace LWP::Math;
using namespace LWP::Object;

// 初期化
void DirectionLight::Initialize() {
	color = Utility::ColorPattern::WHITE;
	direction = { 1.57f,0.0f,0.0f };
	intensity = 1.0f;
	name = "DirectionLight";
}
// 更新
void DirectionLight::Update(Base::CommandManager* manager) {
	if (!isActive) { return; }
	manager;
	//// 6方向のビュープロジェクション
	//Vector3 rotation[6] = {
	//	{1.57f,0.0f,1.57f},
	//	{1.57f,0.0f,-1.57f},
	//	{1.57f,0.0f,0.0f},
	//	{-1.57f,0.0f,0.0f},
	//	{0.0f,0.0f,0.0f},
	//	{3.14f,0.0f,0.0f},
	//};

	//Matrix4x4 viewProjections[6];
	//for (int i = 0; i < 6; i++) {
	//	Matrix4x4 viewMatrix = (Matrix4x4::CreateRotateXYZMatrix(rotation[i]) * Matrix4x4::CreateTranslateMatrix(position)).Inverse();
	//	Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(0.45f, 1, 0.1f, 100.0f);
	//	viewProjections[i] = viewMatrix * projectionMatrix;
	//}

	// CommandManagerにデータを登録
	//manager->SetPointLightData(this, viewProjections);
}

// デバッグ用GUI
void DirectionLight::DebugGUI() {
	transform.DebugGUI();
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat3("Direction", &direction.x, 0.01f);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
}