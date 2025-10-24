#include "Timer.h"

#include "component/Information.h"

namespace LWP::Utility {
	Timer::Timer() {
		name = "Timer" + name; // 名前を登録（事前に番号だけ振られてるので手前に）
	}

	void Timer::Update() {
		if (!isStart_) { return; }

		// 経過時間を加算
		if (!GetEnd()) {
			duration_ += enableTimeScale ? Info::GetDeltaTimeF() : Info::GetDefaultDeltaTimeF();
		}
		else {
			isStart_ = false;
		}
	}

	void Timer::Start() {
		duration_ = 0;
		isStart_ = true;
	}
	void Timer::Stop() {
		isStart_ = false;
	}
	void Timer::Resume() {
		isStart_ = true;
	}
	bool Timer::GetEnd() {
		return duration_ >= targetTime;
	}

	void Timer::DebugGUI() {
		ImGui::SliderFloat("Duration", &duration_, 0.0f, targetTime);
		ImGui::DragFloat("TargetTime", &targetTime, 0.01f);
		if (ImGui::Button("Start")) { Start(); }
		ImGui::SameLine();
		if (ImGui::Button("Stop")) { Stop(); }
		ImGui::SameLine();
		if (ImGui::Button("Resume")) { Resume(); }
		ImGui::Text("Start: %d", isStart_ ? 1 : 0);
		ImGui::Text("End: %d", GetEnd() ? 1 : 0);
	}
}