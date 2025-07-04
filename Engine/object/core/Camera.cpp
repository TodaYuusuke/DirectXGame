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
	Object::Manager::GetInstance()->SetPtr(this);	// ポインタをセット
	
	// リソースの初期化
	constantBuffer_.Init();
	gBuffer_.Init();
	textureResource_.Init();

	pp.Init();
}
Camera::~Camera() {
	Object::Manager::GetInstance()->DeletePtr(this); // ポインタを解除
}

// 初期化
void Camera::Init() {}
// 更新
void Camera::Update() {
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
}

void Camera::SetMainCamera() { Manager::GetInstance()->SetMainCamera(this); }

void Camera::DebugGUI() {
	worldTF.DebugGUI();
	pp.DebugGUI();
	ImGui::DragFloat("FOV", &fov, 0.01f);
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