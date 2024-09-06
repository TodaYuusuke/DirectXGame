#include "LevelData.h"

#include "base/ImGuiManager.h"
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
	rigidModels.clear();
	skinModels.clear();
	colliders.clear();

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
		
		// カメラの回転だけ正常に読み込めないのでコメントアウト中
		// CAMERA（適応対象のカメラの実体がなければ検証しない）
		//if (cameraPtr && type.compare("CAMERA") == 0) {
		//	// トランスフォームのパラメータ読み込み
		//	SetWorldTF(object["transform"], &cameraPtr->transform);
		//}

		// 地形ならば特殊な処理
		if (objName == "Terrain") {
			terrain = std::make_unique<Object::Terrain>();
			terrain->name = "Terrain";
			terrain->LoadModel(object["file_name"].get<std::string>(), LoadWorldTF(object["transform"]));
		}
		// MESH
		else if (type.compare("MESH") == 0) {
			// ファイルパスがあればそのパスを読み込み
			if (object.contains("file_name")) {
				staticModels[objName].LoadShortPath(object["file_name"].get<std::string>());
			}
			// ファイルパスが無いので仮でCubeを読み込み
			else {
				staticModels[objName].LoadCube();
			}

			// トランスフォームのパラメータ読み込み
			Object::TransformQuat wtf = LoadWorldTF(object["transform"]);
			staticModels[objName].ApplyWorldTransform(wtf);

			// コライダーがあれば生成
			if (object.contains("collider")) {
				nlohmann::json& collider = object["collider"];
				if (collider["type"] == "AABB") {
					AABB& aabb = colliders[objName].SetBroadShape(AABB());
					aabb.min = {
						static_cast<float>(collider["min"][0]),
						static_cast<float>(collider["min"][2]),
						static_cast<float>(collider["min"][1]),
					};
					aabb.max = {
						static_cast<float>(collider["max"][0]),
						static_cast<float>(collider["max"][2]),
						static_cast<float>(collider["max"][1]),
					};
					colliders[objName].worldTF = wtf;
				}
			}

		}
	}
}

void LevelData::DebugGUI() {
	if (ImGui::BeginTabItem("LevelData")) {
		static int radioValue = 0;
		// どれを（Rigid, Skinning, Static, Collider）を表示するか選択
		ImGui::RadioButton("Rigid", &radioValue, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Skinning", &radioValue, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Static", &radioValue, 2);
		ImGui::SameLine();
		ImGui::RadioButton("Collider", &radioValue, 3);

		switch (radioValue) {
			case 0:
				RigidDebugGUI();
				break;
			case 1:
				SkinDebugGUI();
				break;
			case 2:
				StaticDebugGUI();
				break;
			case 3:
				ColliderDebugGUI();
				break;
		}
		ImGui::EndTabItem();
	}
}

Object::TransformQuat LevelData::LoadWorldTF(const nlohmann::json& data) {
	Object::TransformQuat wtf;

	// 平行移動（Blenderの座標系からここで変換）
	wtf.translation = {
		static_cast<float>(data["translation"][0]),
		static_cast<float>(data["translation"][2]),
		static_cast<float>(data["translation"][1]),
	};
	// 回転角
	wtf.rotation =
		Quaternion{
			data["rotation"][1],
			data["rotation"][3],
			data["rotation"][0],
			data["rotation"][2],
		};
	wtf.rotation.z *= -1.0f;
	
	// 平行移動
	wtf.scale = {
		static_cast<float>(data["scaling"][0]),
		static_cast<float>(data["scaling"][2]),
		static_cast<float>(data["scaling"][1]),
	};

	return wtf;
}

void LevelData::RigidDebugGUI() {
	// 読み込み済みのモデル一覧
	if (!rigidModels.empty()) {
		std::vector<const char*> itemText;
		float maxSize = 0;	// Textの中で一番長いサイズを保持する
		// 一覧のパス取得
		for (const auto& m : rigidModels) {
			itemText.push_back(m.first.c_str());
			maxSize = std::max<float>(maxSize, static_cast<float>(m.first.size()));	// 現在の長さより大きければ更新
		}
		ImGui::PushItemWidth(maxSize * 8.5f);	// 最大サイズにUIを合わせる
		static int currentItem = 0;
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<std::string, RigidModel>(rigidModels, currentItem)).second.DebugGUI();
	}
}
void LevelData::SkinDebugGUI() {
	// 読み込み済みのモデル一覧
	if (!skinModels.empty()) {
		std::vector<const char*> itemText;
		float maxSize = 0;	// Textの中で一番長いサイズを保持する
		// 一覧のパス取得
		for (const auto& m : skinModels) {
			itemText.push_back(m.first.c_str());
			maxSize = std::max<float>(maxSize, static_cast<float>(m.first.size()));	// 現在の長さより大きければ更新
		}
		ImGui::PushItemWidth(maxSize * 8.5f);	// 最大サイズにUIを合わせる
		static int currentItem = 0;
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<std::string, SkinningModel>(skinModels, currentItem)).second.DebugGUI();
	}
}
void LevelData::StaticDebugGUI() {
	// 読み込み済みのモデル一覧
	if (!staticModels.empty()) {
		std::vector<const char*> itemText;
		float maxSize = 0;	// Textの中で一番長いサイズを保持する
		// 一覧のパス取得
		for (const auto& m : staticModels) {
			itemText.push_back(m.first.c_str());
			maxSize = std::max<float>(maxSize, static_cast<float>(m.first.size()));	// 現在の長さより大きければ更新
		}
		ImGui::PushItemWidth(maxSize * 8.5f);	// 最大サイズにUIを合わせる
		static int currentItem = 0;
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<std::string, StaticModel>(staticModels, currentItem)).second.DebugGUI();
	}
}
void LevelData::ColliderDebugGUI() {
	// 読み込み済みのモデル一覧
	if (!colliders.empty()) {
		std::vector<const char*> itemText;
		float maxSize = 0;	// Textの中で一番長いサイズを保持する
		// 一覧のパス取得
		for (const auto& m : colliders) {
			itemText.push_back(m.first.c_str());
			maxSize = std::max<float>(maxSize, static_cast<float>(m.first.size()));	// 現在の長さより大きければ更新
		}
		ImGui::PushItemWidth(maxSize * 8.5f);	// 最大サイズにUIを合わせる
		static int currentItem = 0;
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<std::string, Collider>(colliders, currentItem)).second.DebugGUI();
	}
}

// 短縮用パス
const std::string LevelData::kDirectoryPath = "resources/level/";
