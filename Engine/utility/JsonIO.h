#pragma once
#include <map>
#include <../Externals/nlohmann/Json.hpp>

#include <variant>
#include "math/Math.h"
#include <string>

// 前方宣言
namespace LWP::Utility {
	struct Item;

	// グループ
	using Group = std::vector<Item>;
	using ItemType = std::variant<
		int32_t*,
		float*, Math::Vector2*, Math::Vector3*,
		std::string*,
		Group
	>;
	// 項目
	struct Item {
		std::string name;
		ItemType value;
	};
}

// 値型で保持されるシリアライズ可能型
template<typename T>
concept JsonSerializableValue =
std::same_as<T, int32_t> ||
std::same_as<T, float> ||
std::same_as<T, LWP::Math::Vector2> ||
std::same_as<T, LWP::Math::Vector3> ||
std::same_as<T, std::string>;

// ポインタで保持されるシリアライズ可能型
template<typename T>
concept JsonSerializablePointer =
std::same_as<T, int32_t*> ||
std::same_as<T, float*> ||
std::same_as<T, LWP::Math::Vector2*> ||
std::same_as<T, LWP::Math::Vector3*> ||
std::same_as<T, std::string*>;

// variant に入る全体型（Group は唯一の実体型）
template<typename T>
concept JsonSerializableVariantType =
JsonSerializablePointer<T> ||
std::same_as<T, LWP::Utility::Group>;

/// <summary>
/// Jsonの入出力を管理するクラス
/// </summary>
namespace LWP::Utility {
	class JsonIO {
	public: // ** メンバ関数 ** //
		/// <summary>
		/// コンストラクタ
		/// </summary>
		JsonIO() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~JsonIO();

		JsonIO& Begin(const std::string& filePath);
		void End();
		JsonIO& BeginGroup(const std::string& groupName);
		JsonIO& EndGroup();
		template<JsonSerializableValue T>
		JsonIO& AddValue(const std::string& name, T* value) {
			assert(!groupStack_.empty() && "Begin() or BeginGroup() must be called first");
			groupStack_.back()->push_back(Item{ name, value });
			return *this;
		}

		/// <summary>
		/// データをファイルに保存
		/// </summary>
		void Save();
		/// <summary>
		/// データをファイルから読み込み
		/// </summary>
		void Load();

		/// <summary>
		/// Debug用のGUI
		/// </summary>
		void DebugGUI();


	private: // ** メンバ変数 ** //
		// グローバル変数の保存先ファイルパス
		const std::string kDirectoryPath_ = "resources/json/";
		// jsonフォルダ直下からのパス
		std::string filePath_ = "";

		// 全データ
		Group data_;
		// データを追加するための一時変数
		std::vector<Group*> groupStack_;


	private: // ** プライベートのメンバ変数 ** //
		
		/// <summary>
		/// varriantの中身がテンプレート型であるかをチェックする関数
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="item"></param>
		/// <returns></returns>
		template<JsonSerializableVariantType T>
		bool VariantCheck(const Item& item) { return std::holds_alternative<T>(item.value); }
		/// <summary>
		/// varriantの中身をテンプレート型に変換する関数
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="item"></param>
		/// <returns></returns>
		template<JsonSerializableVariantType T>
		T* VariantGet(Item& item) { return std::get_if<T>(&item.value); }
		/// <summary>
		/// jsonから任意の型の変数を読み込む関数
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="item"></param>
		/// <returns></returns>
		template<JsonSerializableValue T>
		void VariantLoad(const nlohmann::json::iterator& json, Group& group, const T& value) {
			std::string key = json.key();
			Group::iterator itr = FindGroup(key, group);
			assert(itr != group.end() && ("Not Found \"" + key + "\" Item.").c_str());
			*(*VariantGet<T*>(*itr)) = value;
		}

		/// <summary>
		/// データをファイルに保存
		/// </summary>
		void Save(nlohmann::json& json, Group& group);
		/// <summary>
		/// データをファイルから読み込み
		/// </summary>
		void Load(nlohmann::json& json, Group& group);

		/// <summary>
		/// jsonファイルの存在を確認
		/// </summary>
		void CheckJsonFile();
		/// <summary>
		/// Groupから特定の名前のItemを探す
		/// </summary>
		/// <param name="name"></param>
		/// <param name="group"></param>
		/// <returns></returns>
		Group::iterator FindGroup(const std::string& name, Group& group);

		/// <summary>
		/// Debug用のGUI（再起処理用）
		/// </summary>
		void DebugGUI(Group& group);
	};
}