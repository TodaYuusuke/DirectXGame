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

	Matrix4x4 viewMatrix = Matrix4x4::CreateIdentity4x4();
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, LWP::Engine::GetWindowWidthf(), LWP::Engine::GetWindowHeightf(), 0.0f, 100.0f);
	viewProjectionMatrix2D = viewMatrix * projectionMatrix * cameraMatrix;
	
	viewMatrix = cameraMatrix.Inverse();
	projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(0.45f, LWP::Engine::GetWindowWidthf() / LWP::Engine::GetWindowHeightf(), 0.1f, 100.0f);
	viewProjectionMatrix3D = viewMatrix * projectionMatrix;
}
// 描画
void Camera::Draw() {
	// なし
}