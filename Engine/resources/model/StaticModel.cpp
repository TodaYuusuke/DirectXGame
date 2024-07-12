#include "StaticModel.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;


StaticModel::StaticModel() {}
StaticModel::~StaticModel() {
	// パスが空じゃなかったら消しに行く
	if (!filePath.empty()) {
		// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
		System::engine->resourceManager_->DeletePointer(this, filePath);
	}
}

void StaticModel::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
	ModelData* data = GetModel(filePath);

	// バッファ生成のためにデバイスとSRVを取得する
	GPUDevice* device = System::engine->directXCommon_->GetGPUDevice();
	SRV* srv = System::engine->directXCommon_->GetSRV();

	// ** Bufferのみあれば問題ないので最低限の処理で済ます ** //

	// バッファの生成
	vertexBuffer_ = std::make_unique<StructuredBuffer<Base::OutputVertexStruct>>(data->buffers_.vertex->kMaxSize);	// 頂点
	// バッファの初期化
	vertexBuffer_->Init(device, srv);
	// データのコピー
	// Vertexだけ型が代わるのでfor文
	for (uint32_t i = 0; i < data->buffers_.vertex->kMaxSize; i++) {
		vertexBuffer_->Add(data->buffers_.vertex->data_[i]);
	}

	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略（ポインタセット）
	System::engine->resourceManager_->SetPointer(this, filePath);
}


void StaticModel::ApplyWorldTransform(const Object::TransformQuat& wtf) {
	Matrix4x4 affine = wtf.GetAffineMatrix();
	// 渡されたワールド変換を適応する
	for (uint32_t i = 0; i < vertexBuffer_->kMaxSize; i++) {
		vertexBuffer_->data_[i].ApplyWorldTransform(affine);
	}
}

void StaticModel::DebugGUI() {
	ImGui::Checkbox("isActive", &isActive);
	//if(ImGui::Button("Change WireFrame")) { ChangeFillMode(); }
}