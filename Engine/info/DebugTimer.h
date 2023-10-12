#pragma once
#include <chrono>

namespace LWP::Information {
	class DebugTimer final {
	public: // メンバ関数

		DebugTimer();

		// 初期化
		void Initialize();

		// 計測開始
		void Start();
		// 計測終了
		void End();

		// 今フレームのFPSを取得
		double GetFPS();
		// 処理を開始してから経過した時間を返す関数
		double GetElapsedTimeMS();
		double GetElapsedTimeS();
		double GetElapsedTimeM();
		double GetElapsedTimeH();

		// ImGuiを用いて情報表示
		void DebugGUI();


	private: // メンバ変数

		// プログラム実行開始時間
		std::chrono::steady_clock::time_point programStartTime_{};
		// フレーム開始時間
		std::chrono::steady_clock::time_point frameStartTime_{};
		// フレーム終了時間
		std::chrono::steady_clock::time_point frameEndTime_{};

		// 平均FPS計測用のデータ配列
		const static int kframeDurationSize_ = 30;

		// 次にデータを入れるindex
		short frameDurationIndex_ = 0;
		// フレーム経過時間の配列
		double frameDurations_[kframeDurationSize_]{};

		// 経過フレーム
		long elapsedFrame_ = 0;
		// 60フレームごとのFPS
		double frameRate = 0;
	};
}