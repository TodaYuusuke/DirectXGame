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
	rotate = value.transform.GetRotateMatrix();
	position = value.transform.GetWorldPosition();
	return *this;
}

Camera::Camera() {
	// ほんとはよくない設計なので代替案募集

	// GPUデバイスのポインタ
	GPUDevice* dev = System::engine->directXCommon_->GetGPUDevice();
	// HeapManagerのポインタ
	HeapManager* heaps = System::engine->directXCommon_->GetHeaps();

	// リソースの初期化
	constantBuffer_.Init(dev);
	renderResource_.Init(dev, heaps);
	textureResource_.Init(dev, heaps);
	depthStencil_.Init(dev, heaps);

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
	transform.DebugGUI();
	pp.DebugGUI();
	ImGui::DragFloat("FOV", &fov, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
}

Matrix4x4 Camera::GetViewProjection() const {
	Matrix4x4 viewMatrix = transform.GetAffineMatrix().Inverse();
	float fovF = fov / 100.0f / 2.0f;
	Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(fovF, Info::GetWindowWidthF() / Info::GetWindowHeightF(), 0.1f, 300.0f);
	return viewMatrix * projectionMatrix;
}

Resource::Texture Camera::GetTexture() { return textureResource_; }
