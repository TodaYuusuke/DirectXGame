#include "MapManager.h"

using namespace LWP::Math;
using namespace LWP::Object;

void MapManager::Initialize() {
	for (int y = 0; y < kSubdivision_; y++) {
		for (int x = 0; x < kSubdivision_; x++) {
			maps_[y][x].Initialize();
			// マップの座標をセット
			float scale = GetMapChipScale();
			maps_[y][x].model->transform.translation = { ((x * 1.0f) + 0.5f) * scale, 0.0f,((y * 1.0f) + 0.5f) * scale, };
			maps_[y][x].model->transform.scale = { scale ,0.0f ,scale };

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
							Vector2 vec = Vector2{maps_[y][x].model->transform.translation.x, maps_[y][x].model->transform.translation.z} - shockWaves[i].circle.center;
							maps_[y][x].waveVec = { vec.x, (1.0f * (shockWaves[i].kMaxRadius - shockWaves[i].circle.radius)) ,vec.y };
							maps_[y][x].waveVec = maps_[y][x].waveVec.Normalize();

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
			}
		}
	}
}

bool MapManager::AddShockWave(LWP::Math::Vector3 centerPosition) {
	// 登録地点が既に使用済みならば戻る
	if (shockWaves[shockWaveIndex].isActive) { return false; }

	// 指定地点から衝撃波を飛ばす
	Vector2 pos = { centerPosition.x,centerPosition.z };
	shockWaves[shockWaveIndex].Initialize(pos);
	shockWaveIndex++;
	if (shockWaveIndex >= kMaxShockWaveCount_) {
		shockWaveIndex = 0;
	}

	return true;
}


void MapManager::CheckCollision(IMob* mob) {
	WorldTransform transform = mob->GetTransform();
	Vector3 center = transform.translation;
	Vector3 size = mob->Size * transform.scale / 2.0f;
	Vector2 pos[4] = {
		{center.x - size.x, center.z - size.z},
		{center.x - size.x, center.z + size.z},
		{center.x + size.x, center.z - size.z},
		{center.x + size.x, center.z + size.z}
	};

	// 一番高いパワーをセットする
	float maxPower = 0.0f;
	Vector3 maxPowerVector{};
	float maxPowerMapHeight = 0;
	// 4つ角でチェック
	for (int i = 0; i < 4; i++) {
		int x = static_cast<int>((pos[i].x + kMapRadius_) / ((kMapRadius_ * 2) / kSubdivision_));
		int y = static_cast<int>((pos[i].y + kMapRadius_) / ((kMapRadius_ * 2) / kSubdivision_));
		if (x < 0 || x >= kSubdivision_ || y < 0 || y >= kSubdivision_) { continue; }

		// 高さを確認してヒットしているか確認する
		if (center.y < maps_[y][x].model->transform.scale.y) {
			// 余りにも小さいパワーの場合は無視する
			if (maps_[y][x].wavePower >= 0.01f) {
				// より大きいパワーの時のみ更新する
				if (maxPower < maps_[y][x].wavePower) {
					maxPower = maps_[y][x].wavePower;
					maxPowerVector = maps_[y][x].waveVec;
					maxPowerMapHeight = maps_[y][x].model->transform.scale.y;
				}
				mob->isFlying_ = true;
			}
		}
	}

	// 一番高いパワーを代入
	if (maxPowerMapHeight > 0) {
		// 速度は初期化
		mob->velocity_.y = 0.0f;
		mob->velocity_ += maxPowerVector * (maxPower * 0.029f);
		//mob->model_->transform.translation.y = maxPowerMapHeight;
	}
}