#pragma once
#include <map>
#include <../Externals/nlohmann/Json.hpp>
#include <variant>

#include "math/Math.h"
#include "object/TransformQuat.h"
#include "Color.h"
#include "../functions/StringFunctions.h"

// 前方宣言
namespace LWP::Utility {
	struct Item;

	// グループ
	using Group = std::vector<Item>;
	using ItemType = std::variant<
		bool*,
		int32_t*,
		float*,
		Math::Vector2*, Math::Vector3*, Math::Vector4*,
		Math::Quaternion*,
		Object::TransformQuat*,
		Color*,
		std::string*,
		Group
	>;
	// 項目
	struct Item {
		std::string name;
		ItemType value;
	};

	// 入れ子構造だけのデータを返す用の宣言
	struct NestedString;
	using NestedList = std::vector<NestedString>;

	struct NestedString {
		std::string name;
		NestedList list;
	};
}

// 値型で保持されるシリアライズ可能型
template<typename T>
concept JsonSerializableValue =
std::same_as<T, bool> ||
std::same_as<T, int32_t> ||
std::same_as<T, float> ||
std::same_as<T, LWP::Math::Vector2> ||
std::same_as<T, LWP::Math::Vector3> ||
std::same_as<T, LWP::Math::Vector4> ||
std::same_as<T, LWP::Math::Quaternion> ||
std::same_as<T, LWP::Object::TransformQuat> ||
std::same_as<T, LWP::Utility::Color> ||
std::same_as<T, std::string>;

// ポインタで保持されるシリアライズ可能型
template<typename T>
concept JsonSerializablePointer =
std::same_as<T, bool*> ||
std::same_as<T, int32_t*> ||
std::same_as<T, float*> ||
std::same_as<T, LWP::Math::Vector2*> ||
std::same_as<T, LWP::Math::Vector3*> ||
std::same_as<T, LWP::Math::Vector4*> ||
std::same_as<T, LWP::Math::Quaternion*> ||
std::same_as<T, LWP::Object::TransformQuat*> ||
std::same_as<T, LWP::Utility::Color*> ||
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

		/// <summary>
		/// 初期化
		/// </summary>
		JsonIO& Init(const std::string& filePath);

		/// <summary>
		/// 入れ子構造を開始
		/// </summary>
		/// <param name="groupName">入れ子の名前</param>
		/// <returns></returns>
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
		/// jsonファイルの存在を確認し、存在していればLoad、していなければSaveを呼び出す関数
		/// </summary>
		void CheckJsonFile();

		/// <summary>
		/// Debug用のGUI
		/// </summary>
		void DebugGUI();


	private: // ** メンバ変数 ** //
		// グローバル変数の保存先ファイルパス
		const static std::string kDirectoryPath_;
		// jsonフォルダ直下からのパス
		std::string filePath_ = "";

		// 全データ
		Group data_;
		// データを追加するための一時変数
		std::vector<Group*> groupStack_;

		// 型の名前の列挙子
		enum TypeID {
			bool_,
			int32_t_,
			float_,
			Vector2_,
			Vector3_,
			Vector4_,
			Quaternion_,
			TransformQuat_,
			TransformQuat_T,
			TransformQuat_R,
			TransformQuat_S,
			Color_,
			string_,
			Group_,
			Count
		};
		// 型の名前の文字列
		const static std::string typeNames_[TypeID::Count];

		// 読み込み時にデータが見つからなかった場合に実行する関数（現在変更禁止）
		std::function<bool(std::string)> onNotFoundFunc_ = [](std::string key) {
			// デフォルトでは何もしない
			// グループが見つからなかった場合はログに警告を出して終わらせる
			Log("Warning: " + key + " is not found.\n");
			return false;	// 新しくデータを作るならtrueを、そうでないならfalseを返す
		};

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
		void VariantLoad(const std::string& name, Group& group, const T& value) {
			Group::iterator itr = FindGroup(name, group);
			if(itr == group.end()) {
				// グループが見つからなかったので、onNotFoundFunc_を実行する
				// 返り値がtrueなら処理を続け、falseなら終了する
				if (!onNotFoundFunc_(name)) {
					return;
				}
			}
			assert(itr != group.end() && ("Not Found \"" + name + "\" Item.").c_str());
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


	public: // ** staticなメンバ関数 ** //

		/// <summary>
		/// jsonからデータの入れ子構造だけを取得する関数
		/// </summary>
		/// <returns></returns>
		static NestedList LoadGroupNames(std::string filePath);
	};
}