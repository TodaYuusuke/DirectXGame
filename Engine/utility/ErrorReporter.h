#pragma once
#include <string>
#include <iostream>
#include "MyUtility.h"

namespace LWP::Utility {
	namespace Exception {
		// 読み込みの際に発生するエラー
		void Load(const std::string& filename,	_In_z_ wchar_t const* file, _In_ unsigned line) {
			std::wstring message =
				L"\"" + ConvertString(filename) + L"\" が見つかりませんでした。\n"
				L"\n"
				L" - 発生した地点 - \n"
				+ std::wstring(file) + L"\n"
				+ std::to_wstring(line) + L"行目\n"
				L"\n"
				L" - 考えられる原因 - \n"
				L" ・ファイル名が間違っている\n"
				L" ・ファイルパスが間違っている\n";

			MessageBoxW(NULL, message.c_str(), L"読み込みエラー", MB_ICONERROR);
			std::exit(EXIT_FAILURE); // プログラムを中断する
		}

	};
};

// 自作assert実験中

#define LoadAssert(expression, filename) (void)(                                            \
    (!!(expression)) ||                                                          \
    (LWP::Utility::Exception::Load(filename, _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
)