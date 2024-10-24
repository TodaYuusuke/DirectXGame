#include "LevelData.h"

#include "base/ImGuiManager.h"
#include "utility/MyUtility.h"
#include <fstream>


using namespace LWP;
using namespace LWP::Base;
using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Utility;

LevelData::~LevelData() {
	rigidModels.clear();
	skinModels.clear();
	collisions.clear();
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
	collisions.clear();

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
			LoadTerrain(object);
		}
		// MESH
		else if (type.compare("MESH") == 0) {
			LoadMesh(object, objName);
		}
		// CURVE
		else if (type.compare("CURVE") == 0) {
			LoadCurve(object, objName);
		}
	}
}

void LevelData::SetScaleMultiply(float s) { scale = s; }

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
		ImGui::SameLine();
		ImGui::RadioButton("CatmullRom", &radioValue, 4);

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
			case 4:
				CatmullRomDebugGUI();
				break;
		}
		ImGui::EndTabItem();
	}
}

Vector3 LevelData::LoadVector3(const nlohmann::json& data) {
	// Blenderの座標系からここで変換
	return Vector3(static_cast<float>(data[0]), static_cast<float>(data[2]), static_cast<float>(data[1]));
}
Quaternion LevelData::LoadQuaternion(const nlohmann::json& data) {
	// Blenderの座標系からここで変換
	Quaternion q = Quaternion(data[1], data[3], data[0], data[2]);
	q.z *= -1.0f;
	return q;
}
TransformQuat LevelData::LoadWorldTF(const nlohmann::json& data) {
	TransformQuat wtf;

	// 平行移動
	wtf.translation = LoadVector3(data["translation"]);
	// 回転角
	wtf.rotation = LoadQuaternion(data["rotation"]);
	// スケール
	wtf.scale = LoadVector3(data["scaling"]);

	// 全体の倍率をかける
	wtf.translation *= scale;
	wtf.scale *= scale;

	return wtf;
}
void LevelData::LoadMesh(nlohmann::json& data, const std::string& name) {
	// ファイルパスがあればそのパスを読み込み
	if (data.contains("file_name")) {
		staticModels[name].LoadShortPath("level/" + data["file_name"].get<std::string>());
	}
	// ファイルパスが無いので仮でCubeを読み込み
	else {
		staticModels[name].LoadCube();
	}

	// トランスフォームのパラメータ読み込み
	TransformQuat wtf = LoadWorldTF(data["transform"]);
	staticModels[name].ApplyWorldTransform(wtf);

	// コライダーがあれば生成
	if (data.contains("collider")) {
		LoadCollider(data, name, wtf);
	}

}
void LevelData::LoadCollider(nlohmann::json& data, const std::string& name, const Object::TransformQuat wtf) {
	nlohmann::json& collider = data["collider"];
	if (collider["type"] == "AABB") {	// AABBコライダーを生成
		Collider::AABB& aabb = collisions[name].SetBroadShape(Collider::AABB());
		aabb.min = LoadVector3(collider["min"]) * scale;	// 全体のスケールをかける
		aabb.max = LoadVector3(collider["max"]) * scale;

		collisions[name].worldTF = wtf;
	}
	else if (collider["type"] == "MESH") {
		// Meshコライダーを生成
		Collider::Mesh& m = collisions[name].SetBroadShape(Collider::Mesh());
		m.Create(&staticModels[name]);
	}

	// 名前を付けておく
	collisions[name].name = name;
	// マスク設定
	collisions[name].mask.SetBelongFrag(lwpC::Collider::FieldObject);	// フィールドオブジェクトに分類
	collisions[name].mask.SetHitFrag(lwpC::Collider::ALL ^ (lwpC::Collider::Terrain | lwpC::Collider::FieldObject));	// Terrainとフィールドオブジェクト以外とのみヒットするように
}
void LevelData::LoadCurve(nlohmann::json& data, const std::string& name) {
	nlohmann::json& curveData = data["curve_data"];
	// 全てのポイントデータを取得
	for (const auto& point : curveData) {
		Vector3 pos = LoadVector3(point["point"]) * scale;	// 全体の倍率も掛ける
		catmullRomCurves[name].controlPoints.push_back(pos);
	}
}
void LevelData::LoadTerrain(const nlohmann::json& data) {
	terrain = std::make_unique<Terrain>();
	terrain->name = "Terrain";
	// トランスフォームのパラメータ読み込み
	TransformQuat wtf = LoadWorldTF(data["transform"]);
	terrain->LoadModel("level/" + data["file_name"].get<std::string>(), wtf);
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
	if (!collisions.empty()) {
		std::vector<const char*> itemText;
		float maxSize = 0;	// Textの中で一番長いサイズを保持する
		// 一覧のパス取得
		for (const auto& m : collisions) {
			itemText.push_back(m.first.c_str());
			maxSize = std::max<float>(maxSize, static_cast<float>(m.first.size()));	// 現在の長さより大きければ更新
		}
		ImGui::PushItemWidth(maxSize * 8.5f);	// 最大サイズにUIを合わせる
		static int currentItem = 0;
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<std::string, Collision>(collisions, currentItem)).second.DebugGUI();
	}
}
void LevelData::CatmullRomDebugGUI() {
	// 読み込み済みのモデル一覧
	if (!catmullRomCurves.empty()) {
		std::vector<const char*> itemText;
		// 一覧のパス取得
		for (const auto& c : catmullRomCurves) {
			itemText.push_back(c.first.c_str());
		}
		static int currentItem = 0;
		ImGui::ListBox("List", &currentItem, itemText.data(), static_cast<int>(itemText.size()), 4);

		// 選択された番号のDebugGUIを呼び出す
		(*Utility::GetIteratorAtIndex<std::string, CatmullRom>(catmullRomCurves, currentItem)).second.DebugGUI();
	}
}


// 短縮用パス
const std::string LevelData::kDirectoryPath = "resources/level/";
