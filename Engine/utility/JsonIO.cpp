#include "JsonIO.h"
#include "MyString.h"
#include "base/ImGuiManager.h"
#include <fstream>
#include <filesystem>

#define CHECK_BEGIN() assert(filePath_ != "" && "JsonIO is not initialized. Call Init() first.");

using namespace LWP::Math;
using namespace LWP::Object;
using namespace LWP::Utility;

const std::string JsonIO::typeNames_[] = {
	"bool :",
	"int32_t :",
	"float :",
	"Vector2 :",
	"Vector3 :",
	"Vector4 :",
	"Quaternion :",
	"TransformQuat :",
	typeNames_[TypeID::Vector3_] + "translation",
	typeNames_[TypeID::Quaternion_] + "rotation",
	typeNames_[TypeID::Vector3_] + "scale",
	"Color :",
	"string :",
	"Group :"
};

JsonIO::~JsonIO() {
	// デストラクタ
	// 何もしない
}

JsonIO& JsonIO::Init(const std::string& filePath) {
	filePath_ = filePath;
	data_.clear();	// データの初期化
	groupStack_.clear();
	groupStack_ = { &data_ }; // ルートを積む

	return *this;
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
		MessageBoxA(nullptr, message.c_str(), "JsonIO", 0);
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
		if (VariantCheck<bool*>(*itr)) {
			json[typeNames_[TypeID::bool_] + itr->name] = *(*VariantGet<bool*>(*itr));
		}
		else if (VariantCheck<int32_t*>(*itr)) {
			json[typeNames_[TypeID::int32_t_] + itr->name] = *(*VariantGet<int32_t*>(*itr));
		}
		else if (VariantCheck<float*>(*itr)) {
			json[typeNames_[TypeID::float_] + itr->name] = *(*VariantGet<float*>(*itr));
		}
		else if (VariantCheck<Vector2*>(*itr)) {
			Vector2* ptr = (*VariantGet<Vector2*>(*itr));
			json[typeNames_[TypeID::Vector2_] + itr->name] = nlohmann::json::array({ ptr->x, ptr->y });
		}
		else if (VariantCheck<Vector3*>(*itr)) {
			Vector3* ptr = (*VariantGet<Vector3*>(*itr));
			json[typeNames_[TypeID::Vector3_] + itr->name] = nlohmann::json::array({ ptr->x, ptr->y, ptr->z });
		}
		else if (VariantCheck<Vector4*>(*itr)) {
			Vector4* ptr = (*VariantGet<Vector4*>(*itr));
			json[typeNames_[TypeID::Vector4_] + itr->name] = nlohmann::json::array({ ptr->x, ptr->y, ptr->z, ptr->w });
		}
		else if (VariantCheck<Quaternion*>(*itr)) {
			Quaternion* ptr = (*VariantGet<Quaternion*>(*itr));
			json[typeNames_[TypeID::Quaternion_] + itr->name] = nlohmann::json::array({ ptr->x, ptr->y, ptr->z, ptr->w });
		}
		else if (VariantCheck<TransformQuat*>(*itr)) {
			TransformQuat* ptr = (*VariantGet<TransformQuat*>(*itr));
			nlohmann::json& jsonTF = json[typeNames_[TypeID::TransformQuat_] + itr->name];
			jsonTF = nlohmann::json::object();	// 新しいオブジェクトを作成
			jsonTF[typeNames_[TypeID::TransformQuat_T]] = nlohmann::json::array({ ptr->translation.x, ptr->translation.y, ptr->translation.z });
			jsonTF[typeNames_[TypeID::TransformQuat_R]] = nlohmann::json::array({ ptr->rotation.x, ptr->rotation.y, ptr->rotation.z, ptr->rotation.w });
			jsonTF[typeNames_[TypeID::TransformQuat_S]] = nlohmann::json::array({ ptr->scale.x, ptr->scale.y, ptr->scale.z });
		}
		else if (VariantCheck<Color*>(*itr)) {
			Color* ptr = (*VariantGet<Color*>(*itr));
			json[typeNames_[TypeID::Color_] + itr->name] = nlohmann::json::array({ ptr->R, ptr->G, ptr->B, ptr->A });
		}
		else if (VariantCheck<std::string*>(*itr)) {
			json[typeNames_[TypeID::string_] + itr->name] = *(*VariantGet<std::string*>(*itr));
		}
		else if (VariantCheck<Group>(*itr)) {
			std::string key = typeNames_[TypeID::Group_] + itr->name;
			json[key] = nlohmann::json::object();	// 新しいオブジェクトを作成
			Save(json[key], *VariantGet<Group>(*itr));	// 再帰的に処理を行う
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
		MessageBoxA(nullptr, message.c_str(), "JsonIO", 0);
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
		// キーから型を取得
		std::vector<std::string> typeNameSplit = Split(itr.key(), ':');	// 0 : 型名, 1 : 名前
		std::string type = typeNameSplit[0] + ":";	// 型名の末尾に':'を追加
		std::string name = typeNameSplit[1];

		if (type == typeNames_[bool_]) {
			VariantLoad<bool>(name, group, itr->get<bool>());
		}
		else if (type == typeNames_[int32_t_]) {
			VariantLoad<int32_t>(name, group, itr->get<int32_t>());
		}
		else if (type == typeNames_[float_]) {
			VariantLoad<float>(name, group, static_cast<float>(itr->get<double>()));
		}
		else if (type == typeNames_[Vector2_]) {
			Vector2 value = { itr->at(0), itr->at(1) };
			VariantLoad<Vector2>(name, group, value);
		}
		else if (type == typeNames_[Vector3_]) {
			Vector3 value = { itr->at(0), itr->at(1), itr->at(2) };
			VariantLoad<Vector3>(name, group, value);
		}
		else if (type == typeNames_[Vector4_]) {
			Vector4 value = { itr->at(0), itr->at(1), itr->at(2), itr->at(3) };
			VariantLoad<Vector4>(name, group, value);
		}
		else if (type == typeNames_[Quaternion_]) {
			Quaternion value = { itr->at(0), itr->at(1), itr->at(2), itr->at(3) };
			VariantLoad<Quaternion>(name, group, value);
		}
		else if (type == typeNames_[TransformQuat_]) {
			TransformQuat value;
			// TransformQuatの各要素を取得
			auto itrT = itr->at(typeNames_[TransformQuat_T]);
			auto itrR = itr->at(typeNames_[TransformQuat_R]);
			auto itrS = itr->at(typeNames_[TransformQuat_S]);
			value.translation = { itrT.at(0), itrT.at(1), itrT.at(2) };
			value.rotation = { itrR.at(0), itrR.at(1), itrR.at(2), itrR.at(3) };
			value.scale = { itrS.at(0), itrS.at(1), itrS.at(2) };
			VariantLoad<TransformQuat>(name, group, value);
		}
		else if (type == typeNames_[Color_]) {
			Color value = { static_cast<unsigned char>(itr->at(0)), static_cast<unsigned char>(itr->at(1)), static_cast<unsigned char>(itr->at(2)), static_cast<unsigned char>(itr->at(3)) };
			VariantLoad<Color>(name, group, value);
		}	
		else if (type == typeNames_[string_]) {
			VariantLoad<std::string>(name, group, itr->get<std::string>());
		}
		else if (type == typeNames_[Group_]) {
			Group::iterator gItr = FindGroup(name, group);	// グループを検索
			if (gItr == group.end()) {
				// グループが見つからなかったので、onNotFoundFunc_を実行する
				// 返り値がtrueなら処理を続け、falseなら終了する
				if (!onNotFoundFunc_(name)) {
					continue;
				}
			}
			Load(json[itr.key()], *VariantGet<Group>(*gItr));	// 再帰的に処理を行う
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
	ImGui::SameLine();
	if (ImGui::Button("Load")) { Load(); }
	ImGui::Text("---------- Data ----------");
	if (ImGui::TreeNode(filePath_.c_str())) {
		DebugGUI(data_);
		ImGui::TreePop();
	}
}
void JsonIO::DebugGUI(Group& group) {
	for(auto itr = group.begin(); itr != group.end(); ++itr) {
		if (VariantCheck<bool*>(*itr)) {
			ImGui::Checkbox(itr->name.c_str(), *VariantGet<bool*>(*itr));
		}
		else if (VariantCheck<int32_t*>(*itr)) {
			ImGui::DragInt(itr->name.c_str(), *VariantGet<int32_t*>(*itr));
		}
		else if (VariantCheck<float*>(*itr)) {
			ImGui::DragFloat(itr->name.c_str(), *VariantGet<float*>(*itr), 0.01f);
		}
		else if (VariantCheck<Vector2*>(*itr)) {
			ImGui::DragFloat2(itr->name.c_str(), &(*VariantGet<Vector2*>(*itr))->x, 0.01f);
		}
		else if (VariantCheck<Vector3*>(*itr)) {
			ImGui::DragFloat3(itr->name.c_str(), &(*VariantGet<Vector3*>(*itr))->x, 0.01f);
		}
		else if (VariantCheck<Vector4*>(*itr)) {
			ImGui::DragFloat4(itr->name.c_str(), &(*VariantGet<Vector4*>(*itr))->x, 0.01f);
		}
		else if (VariantCheck<Quaternion*>(*itr)) {
			ImGui::DragFloat4(itr->name.c_str(), &(*VariantGet<Quaternion*>(*itr))->x, 0.01f);
		}
		else if (VariantCheck<TransformQuat*>(*itr)) {
			(*VariantGet<TransformQuat*>(*itr))->DebugGUI(itr->name);
		}
		else if (VariantCheck<Color*>(*itr)) {
			LWP::Base::ImGuiManager::ColorEdit4(itr->name.c_str(), **VariantGet<Color*>(*itr));
		}
		else if (VariantCheck<std::string*>(*itr)) {
			Base::ImGuiManager::InputText(itr->name.c_str(), **VariantGet<std::string*>(*itr), 5);
		}
		else if (VariantCheck<Group>(*itr)) {
			if (ImGui::TreeNode(itr->name.c_str())) {
				DebugGUI(*VariantGet<Group>(*itr));
				ImGui::TreePop();
			}
		}
	}
}

const std::string JsonIO::kDirectoryPath_ = "resources/json/";
NestedList JsonIO::LoadGroupNames(std::string filePath) {
	// 書き込むJSONファイルのフルパスを合成する
	std::string fullPath = kDirectoryPath_ + filePath;
	// 書き込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを書き込み用に開く
	ifs.open(fullPath);

	// ファイルオープン失敗したら 
	if (ifs.fail()) {
		std::string message = "LoadGroupNames: Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "JsonIO", 0);
		return {};	// まだ生成していないものなので何も返さない
	}

	nlohmann::json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	auto lamda = [](auto self, nlohmann::json& json, NestedList& list) -> void {
			for (auto itr = json.begin(); itr != json.end(); ++itr) {
				// キーから型を取得
				std::vector<std::string> typeNameSplit = Split(itr.key(), ':');	// 0 : 型名, 1 : 名前
				std::string type = typeNameSplit[0] + ":";	// 型名の末尾に':'を追加
				std::string name = typeNameSplit[1];

				if (type == typeNames_[Group_]) {
					list.push_back({ name, NestedList() });	// 新しいリストを作成
					self(self, json[itr.key()], list.back().list);
				}
			}
		};

	NestedList nestedList;	// 出力するデータ
	lamda(lamda, root, nestedList);	// 実行
	return nestedList;
}