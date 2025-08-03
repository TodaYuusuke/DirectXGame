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
	Vector3 eye = (dir * -1.0f) * distance;
	Matrix4x4 viewMatrix = Matrix4x4::CreateLookAtMatrix(eye, dir);

	// カメラのワールド座標をライトのビュー空間に変換
	Vector3 cameraInLightView = Object::Manager::GetInstance()->GetMainCamera()->worldTF.GetWorldPosition() * viewMatrix;

	// ビュー空間のカメラ位置を中心に投影範囲を決定
	float left = cameraInLightView.x - range;
	float right = cameraInLightView.x + range;
	float top = cameraInLightView.y - range;  // 注意：top < bottomになるように
	float bottom = cameraInLightView.y + range;
	float nZ = cameraInLightView.z - range;  // ライト空間での奥方向（Z+）
	float fZ = cameraInLightView.z + range;

	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(
		left, top, right, bottom, nZ, fZ
	);

	//Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(
	//	-range + offset.x, -range + offset.y,
	//	 range + offset.x,  range + offset.y,
	//	nearZ, farZ
	//);
	return viewMatrix * projectionMatrix;

	//// 逆変換で視錐台をワールド空間に展開
	//Matrix4x4 camViewProjInv = Object::Manager::GetInstance()->GetMainCamera()->GetViewProjection().Inverse();
	//// NDC上の8点（[-1,+1]範囲）を定義
	//Vector3 ndcCorners[8] = {
	//	{-1, +1, 0}, {+1, +1, 0}, {+1, -1, 0}, {-1, -1, 0}, // near
	//	{-1, +1, 1}, {+1, +1, 1}, {+1, -1, 1}, {-1, -1, 1}, // far
	//};
	//// ワールド空間に変換
	//Vector3 frustumCornersWS[8];
	//for (int i = 0; i < 8; ++i) {
	//	Vector4 p = Vector4{ ndcCorners[i].x,ndcCorners[i].y,ndcCorners[i].z, 1.0f } * camViewProjInv;
	//	frustumCornersWS[i] = p.xyz() / p.w; // 射影除算
	//}

	//// ライトビュー行列を仮に作成（ライト位置を適当でもOK）
	//Vector3 dir = (Vector3{ 0.0f,0.0f,1.0f } *worldTF.rotation).Normalize();	// ライトの向き
	//Vector3 eye = (dir * -1.0f) * distance;
	//Matrix4x4 lightView = Matrix4x4::CreateLookAtMatrix(eye, dir);
	//// 視錐台をライトビュー空間に変換
	//Vector3 frustumCornersLS[8];
	//for (int i = 0; i < 8; ++i) {
	//	frustumCornersLS[i] = frustumCornersWS[i] * lightView;
	//}
	//// AABBのmin/maxを求める
	//Vector3 minLS = frustumCornersLS[0];
	//Vector3 maxLS = frustumCornersLS[0];
	//for (int i = 1; i < 8; ++i) {
	//	minLS = Vector3::Min(minLS, frustumCornersLS[i]);
	//	maxLS = Vector3::Max(maxLS, frustumCornersLS[i]);
	//}

	//Matrix4x4 lightProj = Matrix4x4::CreateOrthographicMatrix(
	//	minLS.x, maxLS.x,
	//	maxLS.y, minLS.y,
	//	-maxLS.z, -minLS.z // 注意：DirectXの左手系だとZは反転
	//);

	//return lightView * lightProj;
}

// デバッグ用GUI
void DirectionLight::DebugGUI() {
	worldTF.DebugGUI();
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::DragFloat("ShadowIntensity", &shadowIntensity, 0.01f);
	ImGui::DragFloat("distance", &distance, 0.01f);
	ImGui::DragFloat("bias", &bias, 0.0000001f, 0.0f, 1.0f, "%.8f");
	ImGui::DragFloat("range", &range, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
	if (ImGui::TreeNode("ShadowMap")) {
		ImGuiManager::ShowShadowMap(shadowMap_);
		ImGui::TreePop();
	}
}