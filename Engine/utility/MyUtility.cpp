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

int LWP::Utility::RadianToDegree(float radian) {
	return static_cast<int>(radian * (180.0f / M_PI));
}

float LWP::Utility::DegreeToRadian(int degree) {
	return static_cast<float>(degree * (M_PI / 180.0f));
}