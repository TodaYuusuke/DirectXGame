#include "Camera.h"
#include "../../../Adapter/Adapter.h"

using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Info;
using namespace LWP::Resource;

// 初期化
void Camera::Initialize() {
	
}
// 更新
void Camera::Update(Base::CommandManager* manager) {
	if (!isActive || !renderTexture_) { return; }
	// カメラがアクティブかつ、レンダリングテクスチャが用意されている場合にViewProjectionをセット
	manager->SetSubRendering(this);
}

void Camera::ReCreateShader() {
	if (!ppRenderer_) { return; }
	ppRenderer_->ReCreateShader(shaderPath);
}

void Camera::DebugGUI() {
	transform.DebugGUI();
	ImGui::DragFloat("FOV", &fov, 0.01f);
	ImGui::Checkbox("isUsePostProcess", &isUsePostProcess);
	ImGui::Checkbox("isActive", &isActive);
}

Matrix4x4 Camera::GetViewProjection() const {
	Matrix4x4 viewMatrix = transform.GetWorldMatrix().Inverse();
	float fovF = fov / 100.0f / 2.0f;
	Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(fovF, LWP::Info::GetWindowWidthF() / LWP::Info::GetWindowHeightF(), 0.1f, 100.0f);
	return viewMatrix * projectionMatrix;
}

void Camera::CreateRenderTexture(Base::DirectXCommon* directX, const int width, const int height) {
	// レンダリング先のテクスチャを生成
	renderTexture_ = new RenderTexture(directX, width, height);
}
