#pragma once
#include <string>
#include <typeinfo>

/// <summary>
/// 文字列関連の便利関数をまとめたもの
/// </summary>

namespace LWP::Utility {
	/// <summary>
	/// ログの表示
	/// </summary>
	void Log(const std::string& message);

	/// <summary>
	/// string -> wstringへの変換
	/// </summary>
	std::wstring ConvertString(const std::string& str);
	/// <summary>
	/// wstring -> stringへの変換
	/// </summary>
	std::string ConvertString(const std::wstring& str);

	/// <summary>
	/// stringから前n文字、後m文字を削除する関数
	/// </summary>
	/// <param name="str"></param>
	/// <param name="n"></param>
	/// <param name="m"></param>
	/// <returns></returns>
	std::string TrimmingString(const std::string& str, std::size_t n, std::size_t m);
	/// <summary>
	/// パスを１つ上のディレクトリパスのパスに変換する関数
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	std::string ConvertToParentDirectory(const std::string& filePath);

	/// <summary>
	/// クラス名をstringで返す関数（クラス名だけver）
	/// </summary>
	/// <param name="type"></param>
	/// <returns></returns>
	std::string GetClassNameShort(const std::type_info& type);
}