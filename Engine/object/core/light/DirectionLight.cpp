#include "DirectionLight.h"

#include "base/ImGuiManager.h"
#include "component/System.h"

#include "Config.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;

DirectionLight::DirectionLight() {
	// ほんとはよくない設計なので代替案募集

	LWP::Information::OutputLog::Output("Create DirectionLight\n");

	// GPUデバイスのポインタ
	GPUDevice* dev = System::engine->directXCommon_->GetGPUDevice();
	// HeapManagerのポインタ
	HeapManager* heaps = System::engine->directXCommon_->GetHeaps();
	
	// リソースの初期化
	viewBuffer_.Init(dev);
	shadowMap_.Init(dev, heaps);
}

// 初期化
void DirectionLight::Initialize() {
	color = Utility::ColorPattern::WHITE;
	rotation = { 1.57f,0.0f,0.0f };
	intensity = 1.0f;
	name = "DirectionLight";
}
// 更新
void DirectionLight::Update(Base::RendererManager* manager) {
	if (!isActive) { return; }

	// データを登録
	manager->AddLightData(this);
	*viewBuffer_.data_ = GetViewProjection();
	manager->AddTarget(viewBuffer_.GetGPUView(), &shadowMap_);
	manager;
}

// ビュープロジェクションを返す関数
Matrix4x4 DirectionLight::GetViewProjection() const {
	// 回転行列を取得
	Matrix4x4 rotateMatrix = Matrix4x4::CreateRotateXYZMatrix(rotation);
	// 正規化された方向ベクトルを取得
	Vector3 norVec = (Vector3{ 0.0f,0.0f,1.0f } *rotateMatrix).Normalize();
	// ライトの向きの逆ベクトルがtranslation
	Vector3 v = -1 * norVec;
	Matrix4x4 translateMatrix = Matrix4x4::CreateTranslateMatrix(v);
	// Viewを計算
	Matrix4x4 viewMatrix = (rotateMatrix * translateMatrix).Inverse();

	// ViewProjectionを生成
	Matrix4x4 projectionMatrix = Matrix4x4::CreateOrthographicMatrix(0.0f, 0.0f, 10240.0f * lwpC::Shadow::kResolutionScale, 10240.0f * lwpC::Shadow::kResolutionScale, -5000.0f, 5000.0f);
	Matrix4x4 viewportMatrix = Matrix4x4::CreateViewportMatrix(0.0f, 0.0f, 1024.0f * lwpC::Shadow::kResolutionScale, 1024.0f * lwpC::Shadow::kResolutionScale, 0.0f, 1.0f);

	return viewMatrix * projectionMatrix * viewportMatrix;
}

// デバッグ用GUI
void DirectionLight::DebugGUI() {
	LWP::Base::ImGuiManager::ColorEdit4("color", color);
	ImGui::DragFloat3("Rotation", &rotation.x, 0.01f);
	ImGui::DragFloat("Intensity", &intensity, 0.01f);
	ImGui::Checkbox("isActive", &isActive);
}