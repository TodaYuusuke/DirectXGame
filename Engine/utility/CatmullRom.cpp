#include "CatmullRom.h"
#include "../base/ImGuiManager.h"
#include <algorithm>

using namespace LWP::Math;
using namespace LWP::Utility;

Vector3 CatmullRom::GetPosition() {
	// 4頂点未満の場合計算しない
	if (controlPoints.size() < 4) {
		assert(false);
	}

	// サイズに応じて計算する4点とtの値を求める
	float calcT = t * (controlPoints.size() - 1);
	// 整数部を取得
	int roop = static_cast<int>(calcT);
	if (t == 1.0f) {
		roop -= 1;
	}
	// 小数部を取得
	if (t == 1.0f) {
		calcT = 1.0f;
	}
	else {
		calcT -= static_cast<float>(roop);
	}

	roop = std::clamp(roop, 0, static_cast<int>(controlPoints.size() - 1));
	calcT = std::clamp(calcT, 0.0f, 1.0f);

	// 計算する4点を求める
	Vector3 pos0{}, pos1{}, pos2{}, pos3{};
	// 最初のイテレータ
	std::vector<Vector3>::iterator itr = controlPoints.begin();
	for (int i = 0; i <= roop; i++) {

		// pos0は始点の場合 -> pos1と同じ
		if (itr == controlPoints.begin()) {
			pos0 = *itr; // 同じ
			pos1 = *itr;
		}
		else {
			pos0 = *--itr; // ここでマイナスしてるので
			pos1 = *++itr; // プラスする
		}
		pos2 = *++itr;
		// pos3は終点の場合 -> pos2と同じ
		if (itr == --controlPoints.end()) {
			pos3 = *itr;
		}
		else {
			pos3 = *++itr;
			--itr; // 戻す
		}
	}

	// 頂点座標を計算
	return Interpolation(pos0, pos1, pos2, pos3, calcT);
}

void CatmullRom::DebugGUI() {
	//LWP::Base::ImGuiManager::ColorEdit4(label.c_str(), *this);
}

// 補間
LWP::Math::Vector3 CatmullRom::Interpolation(LWP::Math::Vector3 p0, LWP::Math::Vector3 p1, LWP::Math::Vector3 p2, LWP::Math::Vector3 p3, float calcT) {
	return (((p0 * -1) + (p1 * 3) + (p2 * -3) + (p3)) * powf(calcT, 3.0f) +
		((p0 * 2) + (p1 * -5) + (p2 * 4) + (p3 * -1)) * powf(calcT, 2.0f) +
		((p0 * -1) + (p2)) * calcT + (p1 * 2)) * (1.0f / 2.0f);
}