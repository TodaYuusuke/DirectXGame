#include "LevelData.h"

#include "utility/MyUtility.h"
#include <fstream>


using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object::Collider;
using namespace LWP::Resource;
using namespace LWP::Utility;

LevelData::~LevelData() {
	rigidModels.clear();
	skinModels.clear();
	for (auto c : colliders) {
		delete c.second;
	}
	colliders.clear();
}

void LevelData::Load(const std::string& fileName) {
	// resourcesフォルダ内から検索して読み込む処理を作る
	assert(false);	// 未実装のためassert
	fileName;
	// フルパスを作って読み込む
	//LoadFullPath();
}
void LevelData::LoadShortPath(const std::string& fp) {
	// フルパスにして読み込む
	LoadFullPath(kDirectoryPath + fp);
}
void LevelData::LoadFullPath(const std::string& fp) {
	// 名前を保持
	filePath = fp;
	// 再読み込みと同じ処理
	HotReload();
}

void LevelData::HotReload() {
	// ファイルストリーム
	std::ifstream file;
	file.open(filePath);
	// ファイルオープンチェック
	if (file.fail()) {
		assert(0);
	}

	// Json文字列から解凍したデータ
	nlohmann::json deserialized;
	// 解凍
	file >> deserialized;

	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));
		assert(object.contains("name"));
		// 各データを受け取る
		std::string type = object["type"].get<std::string>();	// 種別を取得
		std::string objName = object["name"].get<std::string>();	// 名前を受け取る
		
		// CAMERA（適応対象のカメラの実体がなければ検証しない）
		//if (cameraPtr && type.compare("CAMERA") == 0) {
		//	// トランスフォームのパラメータ読み込み
		//	SetWorldTF(object["transform"], &cameraPtr->transform);
		//}

		// MESH
		if (type.compare("MESH") == 0) {
			// ファイルパスがあればそのパスを読み込み
			if (object.contains("file_name")) {
				rigidModels[objName].LoadShortPath(object["file_name"].get<std::string>());
			}
			// ファイルパスが無いので仮でCubeを読み込み
			else {
				rigidModels[objName].LoadCube();
			}

			// トランスフォームのパラメータ読み込み
			SetWorldTF(object["transform"], &rigidModels[objName].worldTF);
		}
	}
}

void LevelData::DebugGUI() {

}

void LevelData::SetWorldTF(const nlohmann::json& data, Object::TransformQuat* target) {
	// 平行移動（Blenderの座標系からここで変換）
	target->translation = {
		static_cast<float>(data["translation"][0]),
		static_cast<float>(data["translation"][2]),
		static_cast<float>(data["translation"][1]),
	};
	// 回転角
	target->rotation =
		/*Quaternion::CreateFromAxisAngle({1.0f,0.0f,0.0f}, -static_cast<float>(M_PI) / 2.0f) **/
		Quaternion{
			data["rotation"][0],
			data["rotation"][2],
			data["rotation"][1],
			data["rotation"][3],
	}.Inverse();
	
	// 平行移動
	target->scale = {
		static_cast<float>(data["scaling"][0]),
		static_cast<float>(data["scaling"][2]),
		static_cast<float>(data["scaling"][1]),
	};
}


// 短縮用パス
const std::string LevelData::kDirectoryPath = "resources/level/";
