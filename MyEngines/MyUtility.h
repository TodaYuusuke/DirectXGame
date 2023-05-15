#pragma once
#include <string>

class MyUtility
{
public: // メンバ関数


	/// <summary>
	/// ログの表示
	/// </summary>
	static void Log(const std::string& message);


	/// <summary>
	/// string -> wstringへの変換
	/// </summary>
	static std::wstring ConvertString(const std::string& str);

	/// <summary>
	/// wstring -> stringへの変換
	/// </summary>
	static std::string ConvertString(const std::wstring& str);

};

