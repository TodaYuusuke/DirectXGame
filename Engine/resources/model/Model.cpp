#include "Model.h"

#include "component/Resource.h"

using namespace LWP::Resource;
using namespace LWP::Math;

void Model::Load(const std::string& fileName) {
	// resourcesフォルダ内から検索して読み込む処理を作る
	assert(false);	// 未実装のためassert
	fileName;
	// フルパスを作って読み込む
	//LoadFullPath();
}
void Model::LoadShortPath(const std::string& filePath) {
	// フルパスにして読み込む
	LoadFullPath(kDirectoryPath + filePath);
}
void Model::LoadFullPath(const std::string& filePath) {
	// リソースマネージャーに読み込んでもらう
	LoadModel(filePath);
}


// 短縮用パス
const std::string Model::kDirectoryPath = "resources/model/";
