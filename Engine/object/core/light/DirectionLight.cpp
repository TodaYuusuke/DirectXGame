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
	color = Utility::ColorPattern::WHITE;
	rotation = { 1.57f,0.0f,0.0f };
	intensity = 1.0f;
}
// 更新
void DirectionLight::Update() {
	if (!isActive) { return; }
	Base::DirectionalLightStruct* data = lightBuffer_.data_;

	data->vp = Object::Manager::GetInstance()->GetMainCamera()->GetViewProjection();	// ビュープロジェクションをセット
	//data->vp = GetViewProjection();	// ビュープロジェクションをセット
	data->color = color.GetVector4();
	data->direction = (Math::Vector3{ 0.0f,0.0f,1.0f } *Math::Matrix4x4::CreateRotateXYZMatrix(rotation)).Normalize();	// ライトの向き
	data->intensity = intensity;

	*viewBuffer_.data_ = Object::Manager::GetInstance()->GetMainCamera()->GetViewProjection();
	//*viewBuffer_.data_ = GetViewProjection();
}

// ビュープロジェクションを返す関数
Matrix4x4 DirectionLight::GetViewProjection() const {
	// 回転行列を取得
	//Matrix4x4 rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(rotation);
	// 正規化された方向ベクトルを取得
	//Vector3 norVec = (Vector3{ 0.0f,0.0f,1.0f } *rotateMatrix).Normalize();
	// ライトの向きの逆ベクトルがtranslation
	//Vector3 v = -1 * norVec;
	//Matrix4x4 translateMatrix = Matrix4x4::CreateTranslateMatrix(v);
	// Viewを計算
	//Matrix4x4 viewMatrix = (rotateMatrix * translateMatrix).Inverse();

	// ViewProjectionを生成
	//Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, 10240.0f * lwpC::Shadow::kResolutionScale, 10240.0f * lwpC::Shadow::kResolutionScale, -1000.0f, 1000.0f);
	//Matrix4x4 viewportMatrix = Matrix4x4::CreateViewportMatrix(0.0f, 0.0f, 1024.0f * lwpC::Shadow::kResolutionScale, 1024.0f * lwpC::Shadow::kResolutionScale, 0.0f, 1.0f);

	//return viewMatrix * projectionMatrix * viewportMatrix;

	//// 影を落とす対象の中心（カメラ位置やプレイヤー位置など）
	//Vector3 shadowCenter = worldTF.GetWorldPosition();

	//// ライト方向（正規化済み）
	//Matrix4x4 rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(rotation);
	//Vector3 lightDir = (Vector3{ 0.0f, 0.0f, 1.0f } *rotateMatrix).Normalize();

	//// シャドウマップの視点をライトの方向にオフセット
	//Vector3 eye = shadowCenter - lightDir * distance;
	//Vector3 target = shadowCenter;
	//Vector3 up = Vector3{ 0.0f, 1.0f, 0.0f };

	//// View行列をLookAtで作成
	//Matrix4x4 viewMatrix = Matrix4x4::LookAt(eye, target, up);

	//// Projection行列を正射影で作成（範囲は動的に調整可能）
	//Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(
	//	-range, range, -range, range, nearZ, farZ
	//);

	////// Viewport行列は**描画パスでは使わない**
	//return viewMatrix * projectionMatrix;

	Vector3 eye = { 0, 10, 0 };      // 上空10mの位置
	Vector3 target = { 0, 0, 0 };    // 原点を見る
	// View行列をLookAtで作成
	Matrix4x4 viewMatrix = Matrix4x4::LookAt(eye, target);

	// 直交投影のサイズ：20x20、深度範囲：0.0～50.0
	float width = 20.0f;
	float height = 20.0f;
	float n = 0.0f;
	float f = 50.0f;
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(-width, height, width, -height, n, f);
	// Viewport行列は**描画パスでは使わない**
	return viewMatrix * projectionMatrix;
}

// デバッグ用GUI
void DirectionLight::DebugGUI() {
	worldTF.DebugGUI();
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::DragFloat("distance", &distance, 0.01f);
	ImGui::DragFloat("range", &range, 0.01f);
	ImGui::DragFloat("nearZ", &nearZ, 0.01f);
	ImGui::DragFloat("farZ", &farZ, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
	if (ImGui::TreeNode("ShadowMap")) {
		ImGuiManager::ShowShadowMap(shadowMap_, 0.5f);
		ImGui::TreePop();
	}
}