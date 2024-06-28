#include "MyUtility.h"
#include <dxgidebug.h>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

void LWP::Utility::Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}


std::wstring LWP::Utility::ConvertString(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0) {
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);

	return result;
}


std::string LWP::Utility::ConvertString(const std::wstring& str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0) {
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);

	return result;
}

std::string LWP::Utility::TrimmingString(const std::string& str, std::size_t n, std::size_t m) {
	// nが文字列の長さを超える場合は、0から文字列全体を削除する
	if (n >= str.size()) {
		return "";
	}

	// mが残りの文字列の長さを超える場合は、最後まで削除する
	if (n + m >= str.size()) {
		return str.substr(n);
	}

	// 前n文字、後m文字を削除した文字列を返す
	return str.substr(n, str.size() - n - m);
}

std::string LWP::Utility::ConvertToParentDirectory(const std::string& filePath) {
	// ファイルパスを逆順にして、最初に見つかった'/'の位置を取得
	size_t dotPos = filePath.rfind('/');

	// '/'が見つかった場合、その位置までの部分文字列を取得
	if (dotPos != std::string::npos) {
		return filePath.substr(0, dotPos + 1);
	}
	else {
		// '.'が見つからない場合はそのまま返す
		return "";
	}
}

std::string LWP::Utility::GetClassNameShort(const std::type_info& type) {
	// まず名前を取得
	std::string result = type.name();

	// 最後の::の位置を検索
	size_t pos = result.rfind("::");
	if (pos != std::string::npos) {
		// 名前空間部分を除いた部分を返す
		result = result.substr(pos + 2); // "::"の直後から末尾までを取得
	}

	// pointerの時につく文字列の位置を検索
	pos = result.rfind(" * __ptr64");
	if (pos != std::string::npos) {
		// pointerの時につく文字列を削除する
		result = result.erase(pos, 10);
	}

	// 文字列を返す
	return result;
}

float LWP::Utility::RadianToDegree(float radian) {
	return radian * (180.0f / static_cast<float>(M_PI));
}

float LWP::Utility::DegreeToRadian(float degree) {
	return degree * (static_cast<float>(M_PI) / 180.0f);
}