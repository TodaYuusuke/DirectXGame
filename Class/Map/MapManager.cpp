#include "MapManager.h"

void MapManager::Initialize() {
	for (int y = 0; y < kSubdivision_; y++) {
		for (int x = 0; x < kSubdivision_; x++) {
			maps_[y][x].Initialize();
			// マップの座標をセット
			float scale = GetMapChipScale();
			maps_[y][x].model->transform.translation = { ((x * 1.0f) + 0.5f) * scale, -1.0f * scale,((y * 1.0f) + 0.5f) * scale, };
			maps_[y][x].model->transform.scale = { scale ,scale ,scale };
			maps_[y][x].defaultScale = scale;

			// 中心が{0, 0}になるように座標をずらす
			maps_[y][x].model->transform.translation -= { kSubdivision_ / 2.0f * scale, 0.0f, kSubdivision_ / 2.0f * scale };

			// 円形に削る
			maps_[y][x].model->isActive = maps_[y][x].IsInCircle({ {0.0f,0.0f}, kMapRadius_ });
			maps_[y][x].isOut = !maps_[y][x].model->isActive;
		}
	}
}

void MapManager::Update() {
	// ショックウェーブ更新処理
	for (int i = 0; i < kMaxShockWaveCount_; i++) {
		shockWaves[i].Update();
	}

	ImGui::Begin("MapManager");
	for (int y = 0; y < kSubdivision_; y++) {
		for (int x = 0; x < kSubdivision_; x++) {
			if (!maps_[y][x].isOut) {
				// 全てのショックウェーブとの当たり判定を取る
				for (int i = 0; i < kMaxShockWaveCount_; i++) {
					// そもそも波が非アクティブの場合 -> ヒットフラグを初期化
					if (!shockWaves[i].isActive) {
						maps_[y][x].isHit[i] = false;
					}
					// 既に検知済みじゃない場合 -> 
					else if (!maps_[y][x].isHit[i]) {
						// 当たり判定チェック
						if (maps_[y][x].IsInCircle(shockWaves[i].circle)) {
							maps_[y][x].isHit[i] = true;
							// 衝撃波の大きさに応じてパワーを与える
							maps_[y][x].wavePower += 10.0f * (shockWaves[i].kMaxRadius - shockWaves[i].circle.radius);

							//// 衝撃波の大きさに応じてパワーを与える
							//if (maps_[y][x].wavePower <= 0.5f) {
							//	maps_[y][x].wavePower = 10.0f * (shockWaves[i].kMaxRadius - shockWaves[i].circle.radius);
							//}
							//// 既にパワーを持っている場合は超強力な合成波にする
							//else {
							//	maps_[y][x].wavePower = 30.0f * (shockWaves[i].kMaxRadius - shockWaves[i].circle.radius);
							//}
						}
					}
				}

				// 更新処理
				maps_[y][x].Update();

				// ImGui
				std::string str = "Map[" + std::to_string(y) + "][" + std::to_string(x) + "]";
				maps_[y][x].model->DebugGUI(str);
				ImGui::DragFloat((str + ".power").c_str(), &maps_[y][x].wavePower, 0.01f);
			}
		}
	}
	ImGui::End();
}

bool MapManager::AddShockWave(LWP::Math::Vector3 centerPosition) {
	// 登録地点が既に使用済みならば戻る
	if (shockWaves[shockWaveIndex].isActive) { return false; }

	// 指定地点から衝撃波を飛ばす
	LWP::Math::Vector2 pos = { centerPosition.x,centerPosition.z };
	shockWaves[shockWaveIndex].Initialize(pos);
	shockWaveIndex++;
	if (shockWaveIndex >= kMaxShockWaveCount_) {
		shockWaveIndex = 0;
	}

	return true;
}
