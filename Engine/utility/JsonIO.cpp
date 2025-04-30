#include "JsonIO.h"
#include "MyString.h"
#include "base/ImGuiManager.h"
#include <fstream>
#include <filesystem>

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
	// jsonが存在しているかチェック
	CheckJsonFile();
}

JsonIO& JsonIO::BeginGroup(const std::string& groupName) {
	CHECK_BEGIN();
	// 新しいグループを作成
	groupStack_.back()->push_back(Item{ groupName, Group()});
	// 新しいグループをスタックに積む
	groupStack_.push_back(VariantGet<Group>(groupStack_.back()->back()));

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
		if (VariantCheck<int32_t*>(*itr)) {
			json[itr->name] = *(*VariantGet<int32_t*>(*itr));
		}
		else if (VariantCheck<float*>(*itr)) {
			json[itr->name] = *(*VariantGet<float*>(*itr));
		}
		else if (VariantCheck<Vector2*>(*itr)) {
			Vector2* ptr = (*VariantGet<Vector2*>(*itr));
			json[itr->name] = nlohmann::json::array({ ptr->x, ptr->y });
		}
		else if (VariantCheck<Vector3*>(*itr)) {
			Vector3* ptr = (*VariantGet<Vector3*>(*itr));
			json[itr->name] = nlohmann::json::array({ ptr->x, ptr->y, ptr->z });
		}
		else if (VariantCheck<std::string*>(*itr)) {
			json[itr->name] = *(*VariantGet<std::string*>(*itr));
		}
		else if (VariantCheck<Group>(*itr)) {
			json[itr->name] = nlohmann::json::object();	// 新しいオブジェクトを作成
			Save(json[itr->name], *VariantGet<Group>(*itr));	// 再帰的に処理を行う
		}
	}
}
void JsonIO::Load() {
	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + filePath_;
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

	nlohmann::json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;	
	// ファイルを閉じる
	ifs.close();

	// 各種項目を読み込み
	Load(root, data_);
}
void JsonIO::Load(nlohmann::json& json, Group& group) {
	for (auto itr = json.begin(); itr != json.end(); ++itr) {
		// int32_t型の値を保持していれば
		if (itr->is_number_integer()) {
			VariantLoad<int32_t>(itr, group, itr->get<int32_t>());
		}
		// float型の値を保持していれば
		else if (itr->is_number_float()) {
			VariantLoad<float>(itr, group, static_cast<float>(itr->get<double>()));
		}
		// 要素数2の配列であれば
		else if (itr->is_array() && itr->size() == 2) {
			Vector2 value = { itr->at(0), itr->at(1) };
			VariantLoad<Vector2>(itr, group, value);
		}
		// 要素数3の配列であれば
		else if (itr->is_array() && itr->size() == 3) {
			Vector3 value = { itr->at(0), itr->at(1), itr->at(2) };
			VariantLoad<Vector3>(itr, group, value);
		}
		// 文字列であれば
		else if (itr->is_string()) {
			VariantLoad<std::string>(itr, group, itr->get<std::string>());
		}
		else if (itr->is_structured()) {
			std::string key = itr.key();
			Group::iterator gItr = FindGroup(key, group);	// グループを検索
			Load(json[key], *VariantGet<Group>(*gItr));	// 再帰的に処理を行う
		}
	}
}

void JsonIO::CheckJsonFile() {
	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath_ + filePath_;
	// ファイルの存在チェック
	if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)) {
		// 既に存在しているならば自動的に読み込み
		Load();
	}
	else {
		// 存在しないならば新規作成
		Save();
	}
}
Group::iterator JsonIO::FindGroup(const std::string& name, Group& group) {
	for (auto itr = group.begin(); itr != group.end(); ++itr) {
		if (itr->name == name) {
			return itr;
		}
	}
	return group.end();

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
		if (VariantCheck<int32_t*>(*itr)) {
			ImGui::DragInt(itr->name.c_str(), *VariantGet<int32_t*>(*itr));
		}
		else if (VariantCheck<float*>(*itr)) {
			ImGui::DragFloat(itr->name.c_str(), *VariantGet<float*>(*itr));
		}
		else if (VariantCheck<Vector2*>(*itr)) {
			ImGui::DragFloat2(itr->name.c_str(), &(*VariantGet<Vector2*>(*itr))->x);
		}
		else if (VariantCheck<Vector3*>(*itr)) {
			ImGui::DragFloat3(itr->name.c_str(), &(*VariantGet<Vector3*>(*itr))->x);
		}
		else if (VariantCheck<std::string*>(*itr)) {
			char c[256];
			std::string** str = VariantGet<std::string*>(*itr);
			strncpy_s(c, sizeof(c), (*str)->c_str(), _TRUNCATE);	// char型に変換
			ImGui::InputText(itr->name.c_str(), c, sizeof(c));
			*(*str) = c;	// 文字列を更新
		}
		else if (VariantCheck<Group>(*itr)) {
			if (ImGui::TreeNode(itr->name.c_str())) {
				DebugGUI(*VariantGet<Group>(*itr));
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