#include "FrameTracker.h"
#include "Adapter.h"
#include "../Engine/utility/MyUtility.h"
#include <thread>
#pragma comment(lib, "winmm.lib")

#include "Config.h"

using namespace LWP::Information;
using namespace LWP::Utility;


FrameTracker::FrameTracker() {
	programStartTime_ = std::chrono::steady_clock::now();
}

void FrameTracker::Initialize() {
	timeBeginPeriod(1);	// システムタイマーの分解能を上げる
		
	// 定数を求める

	// 1/N秒ピッタリの時間
	kMinTime = static_cast<std::chrono::microseconds>(static_cast<uint64_t>(1000000.0f / static_cast<float>(LWP::Config::FrameRate::kFixedFPS)));
	// 1/N秒よりわずかに短い時間
	kMinCheckTime = static_cast<std::chrono::microseconds>(static_cast<uint64_t>(1000000.0f / static_cast<float>(LWP::Config::FrameRate::kFixedFPS + 5)));


	// プログラム実行開始時間
	programStartTime_ = std::chrono::steady_clock::now();
	// フレーム開始時間
	frameStartTime_ = {};
	// フレーム終了時間
	frameEndTime_ = {};

	// 次にデータを入れるindex
	frameDurationIndex_ = 0;
	// フレーム経過時間の配列
	for (int i = 0; i < kFrameDurationSize_; i++) {
		frameDurations_[i] = LWP::Config::FrameRate::kFixedFPS;
	}

	// 経過フレーム
	elapsedFrame_ = 0;
	// 60フレームごとのFPS
	frameRate = 0;
}

void FrameTracker::Start() {
	//frameStartTime_ = std::chrono::steady_clock::now();
}

void FrameTracker::End() {
	frameEndTime_ = std::chrono::steady_clock::now();
	// 経過時間を取得する
	std::chrono::microseconds frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime_ - frameStartTime_);

	// 固定フレームより早い場合 -> 
	if (frameDuration < kMinCheckTime) {
		// 1/N秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - frameStartTime_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	
	// 平均FPSを計算するためにデータを保持
	frameDurations_[frameDurationIndex_++] = static_cast<double>(frameDuration.count()) / 1000000.0;
	if (frameDurationIndex_ >= kFrameDurationSize_) { frameDurationIndex_ = 0; }

	// 経過フレームを足す
	elapsedFrame_++;
	frameStartTime_ = frameEndTime_;
}

double FrameTracker::GetFPS() {
	return CalculateAverage(frameDurations_, kFrameDurationSize_);
}


double FrameTracker::GetElapsedTimeMS() {
	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<double, std::milli> elapsedTime = currentTime - programStartTime_;
	return elapsedTime.count();
}
double FrameTracker::GetElapsedTimeS() {
	return GetElapsedTimeMS() / 1000.0;
}

double FrameTracker::GetElapsedTimeM() {
	return GetElapsedTimeS() / 60.0;
}

double FrameTracker::GetElapsedTimeH() {
	return GetElapsedTimeM() / 60.0;
}

double FrameTracker::GetDeltaTime() {
	// 係数を掛けて返す
	return frameDurations_[frameDurationIndex_ - 1] * deltaFactor;
}
double FrameTracker::GetDefaultDeltaTime() {
	// そのまま返す
	return frameDurations_[frameDurationIndex_ - 1];
}
void FrameTracker::SetDeltaTimeMultiply(float value) {
	deltaFactor = value;
}

void FrameTracker::DebugGUI() {
	if (ImGui::BeginTabItem("Info")) {
		// 60処理ごとに一度取得するFPS
		if (elapsedFrame_ % 60 == 0) {
			frameRate = GetFPS();
		}
		
		ImGui::Text("ElapsedTime ... %02d:%02d:%02d:%02d", (int)GetElapsedTimeH() % 24, (int)GetElapsedTimeM() % 60, (int)GetElapsedTimeS() % 60, (int)GetElapsedTimeMS() % 1000 / 10);
		ImGui::Text("FPS ... %.1f (%.1lf)", frameRate, GetFPS());
		ImGui::Text("FrameTime ... %.1fms (%.1fms)", 1000.0f / frameRate, 1000.0f / GetFPS());
		ImGui::Text("DeltaTime ... %lf", GetDeltaTime());
		ImGui::Text("ElapsedFrame ... %d", elapsedFrame_);
		
		ImGui::EndTabItem();
	}
}