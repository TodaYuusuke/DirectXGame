#include "IModel.h"

#include "base/directX/RendererManager.h"
#include "resources/ResourceManager.h"

#include "component/Resource.h"
#include "component/System.h"

using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Resource;

void IModel::Load(const std::string& fileName) {
	// resourcesフォルダ内から検索して読み込む処理を作る
	assert(false);	// 未実装のためassert
	fileName;
	// フルパスを作って読み込む
	//LoadFullPath();
}
void IModel::LoadShortPath(const std::string& fp) {
	// フルパスにして読み込む
	LoadFullPath(kDirectoryPath + fp);
}

ModelData* IModel::GetModelData() {
	// パスが空じゃなかったら返す
	if (!filePath.empty()) {
		return GetModel(filePath);
	}
	return nullptr;
}

// 短縮用パス
const std::string IModel::kDirectoryPath = "resources/model/";
