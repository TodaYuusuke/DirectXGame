#include "MapManager.h"

void MapManager::Initialize() {
	for (int y = 0; y < kSubdivision_; y++) {
		for (int x = 0; x < kSubdivision_; x++) {
			maps_[y][x].Initialize();
			// マップの座標をセット
			float scale = GetMapChipScale();
			maps_[y][x].model->transform.translation = { ((x * 1.0f) + 0.5f) * scale, -1.0f,((y * 1.0f) + 0.5f) * scale, };
			maps_[y][x].model->transform.scale = { scale ,scale ,scale };

			// 中心が{0, 0}になるように座標をずらす
			maps_[y][x].model->transform.translation -= { kSubdivision_ / 2.0f * scale, 0.0f, kSubdivision_ / 2.0f * scale };

			// 円形に削る
			maps_[y][x].model->isActive = maps_[y][x].IsInCircle({ {0.0f,0.0f}, 1.0f });
		}
	}
}

void MapManager::Update() {
	ImGui::Begin("MapManager");
	for (int y = 0; y < kSubdivision_; y++) {
		for (int x = 0; x < kSubdivision_; x++) {
			maps_[y][x].Update();

			std::string str = "Map[" + std::to_string(y) + "][" + std::to_string(x) + "]";
			maps_[y][x].model->DebugGUI(str);
		}
	}
	ImGui::End();
}