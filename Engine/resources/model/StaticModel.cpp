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
		Resource::Manager::GetInstance()->DeletePointer(this, filePath);
	}
}

void StaticModel::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
	ModelData* data = GetModel(filePath);

	// ** Bufferのみあれば問題ないので最低限の処理で済ます ** //

	// バッファの生成
	vertexBuffer_ = std::make_unique<StructuredBuffer<Base::OutputVertexStruct>>(data->buffers_.vertex->kMaxSize);	// 頂点
	// バッファの初期化
	vertexBuffer_->Init();
	// データのコピー
	// Vertexだけ型が代わるのでfor文
	for (uint32_t i = 0; i < data->buffers_.vertex->kMaxSize; i++) {
		vertexBuffer_->Add(data->buffers_.vertex->data_[i]);
	}

	// ポインタセット
	Resource::Manager::GetInstance()->SetPointer(this, filePath);
}


void StaticModel::ApplyWorldTransform(const Object::TransformQuat& wtf) {
	Matrix4x4 affine = wtf.GetAffineMatrix();
	// 渡されたワールド変換を適応する
	for (uint32_t i = 0; i < vertexBuffer_->kMaxSize; i++) {
		vertexBuffer_->data_[i].ApplyWorldTransform(affine);
	}
}

void StaticModel::DebugGUI() {
	static Object::TransformQuat wtf;
	wtf.DebugGUI();
	if (ImGui::Button("Apply")) {
		ApplyWorldTransform(wtf);
	}
	ImGui::Checkbox("isActive", &isActive);
	//if(ImGui::Button("Change WireFrame")) { ChangeFillMode(); }
}