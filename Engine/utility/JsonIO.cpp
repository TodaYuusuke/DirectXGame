#include "JsonIO.h"
#include "MyString.h"
#include "base/ImGuiManager.h"
#include <fstream>

#define CHECK_BEGIN() assert(filePath_ != "" && "JsonIO is not initialized. Call Init() first.");

using namespace LWP::Math;
using namespace LWP::Utility;

JsonIO::~JsonIO() {
	// デストラクタ
	// 何もしない
}

JsonIO& JsonIO::Begin(const std::string& filePath) {
	data_.clear();	// データの初期化
	filePath_ = filePath;
	groupStack_ = { &data_ }; // ルートを積む
	return *this;
}
void JsonIO::End() {
	groupStack_.clear();
}

JsonIO& JsonIO::BeginGroup(const std::string& groupName) {
	CHECK_BEGIN();
	// 新しいグループを作成
	(*groupStack_.back())[groupName] = Item{ Group() };
	// 新しいグループをスタックに積む
	groupStack_.push_back(std::get_if<Group>(&(&(*groupStack_.back())[groupName])->value));

	return *this;
}
JsonIO& JsonIO::EndGroup() {
	CHECK_BEGIN();
	if (groupStack_.size() > 1) {
		groupStack_.erase(groupStack_.end() - 1);
	}
	else {
		assert(false && "EndGroup() Error. Please call BeginGroup() first.");
	}

	return *this;
}

void JsonIO::Save() {
	// jsonオブジェクトを作成
	nlohmann::json root;
	root = nlohmann::json::object();

	// 各種項目を保存
	Save(root, data_);

	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + filePath_;
	// ディレクトリがなければ作成する
	std::filesystem::path dir(ConvertToParentDirectory(kDirectoryPath_ + filePath_));
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}
	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込み用に開く
	ofs.open(filePath);

	// ファイルオープン失敗？
	if (ofs.fail()) {
		std::string message = "Failed open data file for write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	// ファイルを閉じる
	ofs.close();
}
void JsonIO::Save(nlohmann::json& json, Group& group) {
	for (auto itr = group.begin(); itr != group.end(); ++itr) {
		// グループ名を取得
		const std::string& name = itr->first;
		// グループの参照を取得
		Item& item = itr->second;

		if (VariantCheck<int32_t*>(item)) {
			json[name] = *(*VariantGet<int32_t*>(item));
		}
		else if (VariantCheck<float*>(item)) {
			json[name] = *(*VariantGet<float*>(item));
		}
		else if (VariantCheck<Vector2*>(item)) {
			Vector2* ptr = (*VariantGet<Vector2*>(item));
			json[name] = nlohmann::json::array({ ptr->x, ptr->y });
		}
		else if (VariantCheck<Vector3*>(item)) {
			Vector3* ptr = (*VariantGet<Vector3*>(item));
			json[name] = nlohmann::json::array({ ptr->x, ptr->y, ptr->z });
		}
		else if (VariantCheck<std::string*>(item)) {
			json[name] = *(*VariantGet<std::string*>(item));
		}
		else if (VariantCheck<Group>(item)) {
			json[name] = nlohmann::json::object();	// 新しいオブジェクトを作成
			Save(json[name], *VariantGet<Group>(item));	// 再帰的に処理を行う
		}
	}
}
void JsonIO::Load() {

}

void JsonIO::DebugGUI() {
	if (ImGui::Button("Save")) { Save(); }
	if (ImGui::Button("Load")) { Load(); }
	ImGui::Text("---------- Data ----------");
	if (ImGui::TreeNode(filePath_.c_str())) {
		DebugGUI(data_);
		ImGui::TreePop();
	}
}
void JsonIO::DebugGUI(Group& group) {
	for(auto itr = group.begin(); itr != group.end(); ++itr) {
		// グループ名を取得
		const std::string& name = itr->first;
		// グループの参照を取得
		Item& item = itr->second;
		

		if (VariantCheck<int32_t*>(item)) {
			ImGui::DragInt(name.c_str(), *VariantGet<int32_t*>(item));
		}
		else if (VariantCheck<float*>(item)) {
			ImGui::DragFloat(name.c_str(), *VariantGet<float*>(item));
		}
		else if (VariantCheck<Vector2*>(item)) {
			ImGui::DragFloat2(name.c_str(), &(*VariantGet<Vector2*>(item))->x);
		}
		else if (VariantCheck<Vector3*>(item)) {
			ImGui::DragFloat3(name.c_str(), &(*VariantGet<Vector3*>(item))->x);
		}
		else if (VariantCheck<std::string*>(item)) {
			char c[256];
			std::string** str = VariantGet<std::string*>(item);
			strncpy_s(c, sizeof(c), (*str)->c_str(), _TRUNCATE);	// char型に変換
			ImGui::InputText(name.c_str(), c, sizeof(c));
			*(*str) = c;	// 文字列を更新
		}
		else if (VariantCheck<Group>(item)) {
			if (ImGui::TreeNode(name.c_str())) {
				DebugGUI(*VariantGet<Group>(item));
				ImGui::TreePop();
			}
		}
	}
}
/*

void GlobalVariables::LoadFiles() {
	// ディレクトリがなければスキップする
	std::filesystem::path dir(kDirectortyPath);
	if (!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(dir);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		// ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		// .jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		// ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

void GlobalVariables::LoadFile(const std::string& groupName) {
	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectortyPath + groupName + ".json";
	// 書き込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを書き込み用に開く
	ifs.open(filePath);

	// ファイルオープン失敗？
	if (ifs.fail()) {
		std::string message = "Failed open data file for write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// グループを検索
	json::iterator itGroup = root.find(groupName);
	// 未登録チェック
	assert(itGroup != root.end());

	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		// アイテム名を取得
		const std::string& itemName = itItem.key();

		// int32_t型の値を保持していれば
		if (itItem->is_number_integer()) {
			// int型の値を登録
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		// float型の値を保持していれば
		else if (itItem->is_number_float()) {
			// float型の値を登録
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		// 要素数3の配列であれば
		else if (itItem->is_array() && itItem->size() == 3) {
			// float型のjson配列登録
			Vector3 value = {itItem->at(0), itItem->at(1), itItem->at(2)};
			SetValue(groupName, itemName, value);
		}
	}
}
*/