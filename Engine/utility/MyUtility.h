#pragma once
#include <string>

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

};
