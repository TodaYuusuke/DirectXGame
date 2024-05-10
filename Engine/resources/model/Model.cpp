#include "Model.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"
#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Resource;

Model::Model() {
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
	System::engine->resourceManager_->SetPointer(this);
}
Model::~Model() {
	// いちいちcomponent/Resource.hに関数書きにいくのがめんどうなので省略
	System::engine->resourceManager_->DeletePointer(this);
}

void Model::Load(const std::string& fileName) {
	// resourcesフォルダ内から検索して読み込む処理を作る
	assert(false);	// 未実装のためassert
	fileName;
	// フルパスを作って読み込む
	//LoadFullPath();
}
void Model::LoadShortPath(const std::string& fp) {
	// フルパスにして読み込む
	LoadFullPath(kDirectoryPath + fp);
}
void Model::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
}

void Model::Draw(Base::RendererManager* render, Resource::Manager* resource) {
	render->AddModelData(resource->GetModelData(filePath), *this);
}
void Model::DebugGUI() {
	worldTF.DebugGUI();
	ImGui::Checkbox("enableLighting", &enableLighting);
	//ImGui::Checkbox("isWireFrame", &isWireFrame);
	ImGui::Checkbox("isActive", &isActive);
}

// 短縮用パス
const std::string Model::kDirectoryPath = "resources/model/";
