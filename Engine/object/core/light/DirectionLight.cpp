#include "DirectionLight.h"

#include <base/ImGuiManager.h>
#include <base/directX/command/CommandManager.h>

#include <Config.h>

using namespace LWP::Math;
using namespace LWP::Object;

// 初期化
void DirectionLight::Initialize() {
	color = Utility::ColorPattern::WHITE;
	rotation = { 1.57f,0.0f,0.0f };
	intensity = 1.0f;
	name = "DirectionLight";
}
// 更新
void DirectionLight::Update(Base::CommandManager* manager) {
	if (!isActive) { return; }
	
	// viewProjectionを計算

	// 回転行列を取得
	Matrix4x4 rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(rotation);
	// 正規化された方向ベクトルを取得
	Vector3 norVec = (Vector3{ 0.0f,0.0f,1.0f } *rotateMatrix).Normalize();
	// ライトの向きの逆ベクトルがtranslation
	Vector3 v = -1 * norVec;
	Matrix4x4 translateMatrix = Matrix4x4::CreateTranslateMatrix(v);
	// Viewを計算
	Matrix4x4 viewMatrix = (rotateMatrix * translateMatrix).Inverse();

	// ViewProjectionを生成
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, 10240.0f * lwpC::Shadow::kResolutionScale, 10240.0f * lwpC::Shadow::kResolutionScale, -5000.0f, 5000.0f);
	Matrix4x4 viewportMatrix = Matrix4x4::CreateViewportMatrix(0.0f, 0.0f, 1024.0f * lwpC::Shadow::kResolutionScale, 1024.0f * lwpC::Shadow::kResolutionScale, 0.0f, 1.0f);

	Matrix4x4 viewProjection = viewMatrix * projectionMatrix * viewportMatrix;

	// CommandManagerにデータを登録
	manager->SetDirectionLightData(this, viewProjection);
}

// デバッグ用GUI
void DirectionLight::DebugGUI() {
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
}