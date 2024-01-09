#pragma once
#include <variant>
#include <map>
#include <string>
#include "Vector3.h"
#include <json.hpp>

/// <summary>
/// グローバル変数
/// </summary>
class GlobalVariables {
public:
	static GlobalVariables* GetInstance();

	/// <summary>
	/// グループの作成
	/// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	// 値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	// 値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	// 値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	// 値の追加(int)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	// 値の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	// 値の追加(Vector3)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

	// 値の取得
	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;


	// グループをファイルに保存
	void SaveFile(const std::string& groupName);
	// ディレクトリの全ファイル読み込み
	void LoadFiles();
	// ファイルから読み込む
	void LoadFile(const std::string& groupName);

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	const GlobalVariables& operator=(const GlobalVariables&) = delete;

	// グローバル変数の保存先ファイルパス
	const std::string kDirectortyPath = "Resources/GlobalVariables/";
	using json = nlohmann::json;

	// 項目
	using Item = std::variant<int32_t, float, Vector3>;
	// グループ
	using Group = std::map<std::string, Item>;

	// 全データ
	std::map<std::string, Group> datas_;
};