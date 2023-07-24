#include "Camera.h"
#include "../../../Adapter/LWP.h"

using namespace LWP::Object;
using namespace LWP::Math;

// 初期化
void Camera::Initialize() {
	transform.Initialize();
}
// 更新
void Camera::Update() {
	// 各行列の計算
	Matrix4x4 cameraMatrix = transform.GetMatWorld();
	Matrix4x4 viewMatrix = cameraMatrix.Inverse();
	Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(0.45f, static_cast<float>(*LWP::Engine::kWindowWidth) / static_cast<float>(*LWP::Engine::kWindowHeight), 0.1f, 100.0f);
	Matrix4x4 viewProjectionMatrix = viewMatrix * projectionMatrix;
	worldViewProjectionMatrix = Matrix4x4::CreateViewportMatrix(0, 0, static_cast<float>(*LWP::Engine::kWindowWidth), static_cast<float>(*LWP::Engine::kWindowHeight), 0.0f, 1.0f);
}
// 描画
void Camera::Draw() {
	// なし
}