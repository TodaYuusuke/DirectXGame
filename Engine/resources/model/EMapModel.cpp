#include "EMapModel.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;


EMapModel::EMapModel() {}
EMapModel::EMapModel(const EMapModel& other) {
	this->LoadFullPath(other.filePath);
	worldTF = other.worldTF;
	isActive = other.isActive;
}
EMapModel::~EMapModel() {
	// パスが空じゃなかったら消しに行く
	if (!filePath.empty()) {
		// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
		System::engine->resourceManager_->DeletePointer(this, filePath);
	}
}

void EMapModel::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
	// マテリアルをコピー
	ModelData* data = GetModel(filePath);
	materials.resize(data->materials_.size());
	std::copy(data->materials_.begin(), data->materials_.end(), materials.begin());

	// モデルの中心座標を計算
	std::vector<Primitive::Vertex> vertices = data->GetVertices();
	Vector3 min = vertices[0].position;
	Vector3 max = min;
	for (int i = 0; i < vertices.size(); i++) {
		const Vector3& v = vertices[i].position;
		min.x = std::min<float>(v.x, min.x);
		min.y = std::min<float>(v.y, min.y);
		min.z = std::min<float>(v.z, min.z);
		max.x = std::max<float>(v.x, max.x);
		max.y = std::max<float>(v.y, max.y);
		max.z = std::max<float>(v.z, max.z);
	}
	modelCenterPosition = (min + max) / 2.0f;;


	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略（ポインタセット）
	System::engine->resourceManager_->SetPointer(this, filePath);

	// バッファ生成のためにデバイスとSRVを取得する
	GPUDevice* device = System::engine->directXCommon_->GetGPUDevice();
	HeapManager* heaps = System::engine->directXCommon_->GetHeaps();

	// リソースの初期化
	cubeMap.Init(device, heaps);
	depthCubeMap.Init(device, heaps);
	for (int i = 0; i < 6; i++) {
		viewBuffers[i].Init(device);
	}
}


void EMapModel::Update() {
	if (!isActive) { return; }

	// 6方向のビュープロジェクション
	static Vector3 rotation[6] = {
		{0.0f,1.57f,0.0f},
		{0.0f,-1.57f,0.0f},
		{-1.57f,0.0f,0.0f},
		{1.57f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,-3.14f,0.0f},
	};

	for (int i = 0; i < 6; i++) {
		Vector3 worldPosition = modelCenterPosition * worldTF.GetAffineMatrix();
		Matrix4x4 viewMatrix = (Matrix4x4::CreateRotateXYZMatrix(rotation[i]) * Matrix4x4::CreateTranslateMatrix(worldPosition)).Inverse();
		Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(1.571f, 1.0f, 0.01f, 300.0f);
		*viewBuffers[i].data_ = viewMatrix * projectionMatrix;

		//Vector3 worldPosition = modelCenterPosition * worldTF.GetAffineMatrix();
		//Matrix4x4 viewMatrix = (Matrix4x4::CreateRotateXYZMatrix(rotation[i]) * Matrix4x4::CreateTranslateMatrix(worldPosition)).Inverse();
		//Matrix4x4 projectionMatrix = Matrix4x4::CreatePerspectiveFovMatrix(1.571f, 1.0f, 0.01f, 300.0f);
		//*viewBuffers[i].data_ = viewMatrix * projectionMatrix;
	}
}
void EMapModel::DebugGUI() {
	worldTF.DebugGUI();
	if (ImGui::TreeNode("Materials")) {
		for (int i = 0; i < materials.size(); i++) {
			materials[i].DebugGUI(std::to_string(i));
		}
		ImGui::TreePop();
	}
	ImGui::Checkbox("isActive", &isActive);
	if (ImGui::Button("Change All Lighting Flag true")) { SetAllMaterialLighting(true); }
	if (ImGui::Button("Change All Lighting Flag false")) { SetAllMaterialLighting(false); }
}

void EMapModel::SetAllMaterialLighting(bool flag) {
	for (Primitive::Material& m : materials) {
		m.enableLighting = flag;
	}
}