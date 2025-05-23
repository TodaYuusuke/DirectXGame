#include "Camera.h"

#include "component/Information.h"
#include "component/System.h"

#include "resources/texture/Texture.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Object;
using namespace LWP::Math;
using namespace LWP::Resource;

CameraStruct& CameraStruct::operator=(const Object::Camera& value) {
	viewProjection = value.GetViewProjection();
	rotate = value.worldTF.GetRotateMatrix();
	position = value.worldTF.GetWorldPosition();
	return *this;
}

Camera::Camera() {
	// リソースの初期化
	constantBuffer_.Init();
	renderResource_.Init();
	textureResource_.Init();
	depthStencil_.Init();

	pp.Init();
}

// 初期化
void Camera::Initialize() {}
// 更新
void Camera::Update(Base::RendererManager* manager) {
	// リソースにデータをコピー
	*constantBuffer_.data_ = *this;
	if (pp.use) {
		// データ更新
		pp.Update();
		Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(fov / 200.0f, Info::GetWindowWidthF() / Info::GetWindowHeightF(), 0.1f, 300.0f);
		pp.outLine.SetProjectionInverse(projectionMatrix.Inverse());
	}

	// isActiveがfalseならレンダリングはしない
	if (!isActive) { return; }

	// カメラがアクティブかつ、レンダリングテクスチャが用意されている場合にViewProjectionをセット
	manager->AddTarget(constantBuffer_.GetGPUView(), &renderResource_, &depthStencil_);
	// ポストプロセスを行うか確認
	if (pp.use) {
		// ポストプロセス用のターゲットセット
		manager->AddTarget(&renderResource_, &textureResource_, &depthStencil_, &pp);
	}
	// しないならば
	else {
	// レンダリング結果をテクスチャ用にコピーする
		manager->AddCopyTask(&renderResource_, &textureResource_);
	}
}

void Camera::DebugGUI() {
	worldTF.DebugGUI();
	pp.DebugGUI();
	ImGui::DragFloat("FOV", &fov, 0.01f);
	if (ImGui::TreeNode("RenderResource")) {
		ImGuiManager::ShowRenderResource(renderResource_, 0.2f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("TextureResource")) {
		ImGuiManager::ShowRenderResource(textureResource_, 0.2f);
		ImGui::TreePop();
	}
	ImGui::Checkbox("isActive", &isActive);
}

Matrix4x4 Camera::GetViewProjection() const {
	Matrix4x4 viewMatrix = worldTF.GetAffineMatrix().Inverse();
	Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(fov / 200.0f, Info::GetWindowWidthF() / Info::GetWindowHeightF(), 0.1f, 300.0f);
	return viewMatrix * projectionMatrix;
}

Resource::Texture Camera::GetTexture() { return textureResource_; }
