#include "DirectionLight.h"

#include "base/ImGuiManager.h"
#include "component/System.h"

#include "Config.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;

DirectionLight::DirectionLight() {
	// リソースの初期化
	lightBuffer_.Init();
	viewBuffer_.Init();
	shadowMap_.Init();
}
DirectionLight::~DirectionLight() {}
// 初期化
void DirectionLight::Init() {
	worldTF.Init();
	color = Utility::ColorPattern::WHITE;
	rotation = { 1.57f,0.0f,0.0f };
	intensity = 1.0f;
}
// 更新
void DirectionLight::Update() {
	if (!isActive) { return; }
	Base::DirectionalLightStruct* data = lightBuffer_.data_;

	data->vp = GetViewProjection();	// ビュープロジェクションをセット
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(
		-range, -range,
		range, range,
		nearZ, farZ
	);
	data-> projectionInverse = projectionMatrix.Inverse();	// 深度値の修正用に逆行列をセット
	data->color = color.GetVector4();
	//data->direction = (Math::Vector3{ 0.0f,0.0f,1.0f } *Math::Matrix4x4::CreateRotateXYZMatrix(rotation)).Normalize();	// ライトの向き
	data->direction = (Vector3{ 0.0f,0.0f,-1.0f } * worldTF.rotation).Normalize();	// ライトの向き
	//data->direction = (Vector3{ 0.0f,0.0f,-1.0f } *Matrix4x4::CreateRotateXYZMatrix(rotation)).Normalize();	// ライトの向き
	data->intensity = intensity;
	data->shadowIntensity = shadowIntensity;
	data->bias = bias;

	*viewBuffer_.data_ = GetViewProjection();
}

// ビュープロジェクションを返す関数
Matrix4x4 DirectionLight::GetViewProjection() const {
	//// 回転行列を取得
	//Matrix4x4 rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(rotation);
	//// 正規化された方向ベクトルを取得
	//Vector3 norVec = (Vector3{ 0.0f,0.0f,1.0f } *rotateMatrix).Normalize();
	//// ライトの向きの逆ベクトルがtranslation
	//Vector3 v = -1 * norVec;
	//Matrix4x4 translateMatrix = Matrix4x4::CreateTranslateMatrix(v);
	//// Viewを計算
	//Matrix4x4 viewMatrix = (rotateMatrix * translateMatrix).Inverse();

	//// ViewProjectionを生成
	//Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, 10240.0f * lwpC::Shadow::kResolutionScale, 10240.0f * lwpC::Shadow::kResolutionScale, -1000.0f, 1000.0f);
	//Matrix4x4 viewportMatrix = Matrix4x4::CreateViewportMatrix(0.0f, 0.0f, 1024.0f * lwpC::Shadow::kResolutionScale, 1024.0f * lwpC::Shadow::kResolutionScale, 0.0f, 1.0f);

	//return viewMatrix * projectionMatrix * viewportMatrix;

	// ↑　以前平行光源のシャドウマップを実装した際とまったく同じコード ↑
	// ↓　試行錯誤の跡 ↓

	//Matrix4x4 viewMatrix = worldTF.GetAffineMatrix().Inverse();
	
	//Vector3 dir = (Vector3{ 0.0f,0.0f,-1.0f } *Matrix4x4::CreateRotateXYZMatrix(rotation)).Normalize();	// ライトの向き
	Vector3 dir = (Vector3{ 0.0f,0.0f,-1.0f } *worldTF.rotation).Normalize();	// ライトの向き
	Matrix4x4 viewMatrix = Matrix4x4::CreateLookAtMatrix({ 0.0f,0.0f,0.0f }, dir);
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(
		-range, -range,
		range, range,
		nearZ, farZ
	);
	return viewMatrix * projectionMatrix;

	//Matrix4x4 viewMatrix = worldTF.GetAffineMatrix().Inverse();
	//Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(fov / 200.0f, Info::GetWindowWidthF() / Info::GetWindowHeightF(), 0.1f, 300.0f);
	//return viewMatrix * projectionMatrix;
}

// デバッグ用GUI
void DirectionLight::DebugGUI() {
	worldTF.DebugGUI();
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::DragFloat("ShadowIntensity", &shadowIntensity, 0.01f);
	ImGui::DragFloat("distance", &bias, 0.0000001f, 0.0f, 1.0f, "%.8f");
	//ImGui::SliderFloat("distance", &distance, 0.0f, 1.0f, "%.8f");
	ImGui::DragFloat("range", &range, 0.01f);
	ImGui::DragFloat("nearZ", &nearZ, 0.01f);
	ImGui::DragFloat("farZ", &farZ, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
	if (ImGui::TreeNode("ShadowMap")) {
		ImGuiManager::ShowShadowMap(shadowMap_);
		ImGui::TreePop();
	}
}