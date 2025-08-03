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
	data->direction = (Vector3{ 0.0f,0.0f,1.0f } * worldTF.rotation).Normalize();	// ライトの向き
	data->intensity = intensity;
	data->shadowIntensity = shadowIntensity;
	data->bias = bias;

	*viewBuffer_.data_ = GetViewProjection();
}

// ビュープロジェクションを返す関数
Matrix4x4 DirectionLight::GetViewProjection() const {
	Vector3 dir = (Vector3{ 0.0f,0.0f,1.0f } *worldTF.rotation).Normalize();	// ライトの向き
	Matrix4x4 viewMatrix = Matrix4x4::CreateLookAtMatrix((dir * -1.0f) * distance, dir);
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(
		-range, -range,
		range, range,
		nearZ, farZ
	);
	return viewMatrix * projectionMatrix;
}

// デバッグ用GUI
void DirectionLight::DebugGUI() {
	worldTF.DebugGUI();
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::DragFloat("ShadowIntensity", &shadowIntensity, 0.01f);
	ImGui::DragFloat("distance", &distance, 0.01f);
	ImGui::DragFloat("bias", &bias, 0.0000001f, 0.0f, 1.0f, "%.8f");
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