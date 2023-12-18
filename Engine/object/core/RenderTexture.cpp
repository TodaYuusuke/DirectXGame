#include "Camera.h"
#include "../../../Adapter/Adapter.h"

using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Info;

// 初期化
void Camera::Initialize() {
	transform.Initialize();
}
// 更新
void Camera::Update(Base::CommandManager* manager) {
	if (!isActive) { return; }
	manager; // マルチレンダリングを実装する際に再定義
	//manager->SetCameraViewProjection(this);
}

void Camera::DebugGUI() {
	transform.DebugGUI();
	ImGui::DragInt("FOV", &fov, 1);
	ImGui::Checkbox("isActive", &isActive);
}

Matrix4x4 Camera::GetViewProjectionMatrix3D() const {
	Matrix4x4 viewMatrix = transform.GetWorldMatrix().Inverse();
	float fovF = static_cast<float>(fov) / 100.0f / 2.0f;
	Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(fovF, LWP::Info::GetWindowWidthF() / LWP::Info::GetWindowHeightF(), 0.1f, 100.0f);
	return viewMatrix * projectionMatrix;
}
Matrix4x4 Camera::GetViewProjectionMatrix2D() const {
	Matrix4x4 viewMatrix = Matrix4x4::CreateIdentity4x4();
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, LWP::Info::GetWindowWidthF(), LWP::Info::GetWindowHeightF(), 0.0f, 100.0f);
	return viewMatrix * projectionMatrix;
}