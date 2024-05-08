#pragma once
#include <fstream>
#include <string>
#include <time.h>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <direct.h>

namespace LWP::Information {
	/// <summary>
	/// デバッグするためのログを外部に出力するクラス
	///	<para>めんどくさいので全部Staticで作ります</para>
	/// </summary>
	class OutputLog final {
	public: // メンバ関数

		static OutputLog* GetInstance() {
			static OutputLog instance;
			return &instance;
		}

		OutputLog() {	
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::tm tm;
			localtime_s(&tm, &now);

			// フォルダを生成
			_mkdir("log");

			// ファイルパスを生成
			std::stringstream temp;
			temp << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
			filePath = "log/" + temp.str() + "_Log.txt";
		}
		~OutputLog() {}


		static void Output(std::string str) { GetInstance()->WriteToFile(str); }

		// ファイルに書き込む処理
		void WriteToFile(std::string str) {
			// ファイルをオープン
			logFile.open(filePath, std::ios_base::app);
			// strを書き込む
			GetInstance()->logFile << str;
			// ファイルをクローズ
			logFile.close();
		}


	private: // ** メンバ定数 ** //

		// ファイルパス
		std::string filePath;
		// ファイルストリーム
		std::ofstream logFile;
	};
}