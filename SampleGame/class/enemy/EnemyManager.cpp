#include "EnemyManager.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace LWP::Info;

void EnemyManager::Init(LevelData* level, Player* player) {
	// ポインタを保持
	player_ = player;

	// 敵のスポーン地点を保存
	spawnPoint_[0].push_back(&level->catmullRomCurves["EnemySP0.000"]);
	spawnPoint_[0].push_back(&level->catmullRomCurves["EnemySP0.001"]);
	spawnPoint_[0].push_back(&level->catmullRomCurves["EnemySP0.002"]);
	spawnPoint_[0].push_back(&level->catmullRomCurves["EnemySP0.003"]);
	spawnPoint_[0].push_back(&level->catmullRomCurves["EnemySP0.004"]);
}

void EnemyManager::Update() {
	// ウェーブが-1ならウェーブ開始していないので早期リターン
	if (wave_ == -1) { return; }

	// 敵更新
	for (SpawnPoint& s : spawnPoint_[wave_]) {
		// nullならばなにもしない
		if (!s.enemy) { continue; }

		// もし死亡しているならば削除
		if (s.enemy->GetIsDead()) {
			delete s.enemy;
			s.enemy = nullptr;
			killCount_++;	// キルカウント+1
			spawnedEnemyCount_--;	// スポーン済みカウント-1
		}
		// そうでなければ通常の更新
		else { s.enemy->Update(); }
	}

	// -- 敵召喚処理 -- //

	// スポーン最大数に到達している場合
	if (spawnedEnemyCount_ >= GetSpawnPointSize()) { return; }	// 処理終了
	// 現在スポーンしている数とキル済みの数の合計が、ノルマに到達している場合
	if (spawnedEnemyCount_ + killCount_ >= quota_) { return; }	// 処理終了


	// スポーンインターバルを更新
	summonInterval_ -= GetDeltaTimeF();
	if (summonInterval_ < 0.0f) {
		summonInterval_ = kSummonInterval_;	// インターバルを最大値に
		Spawn();	// 召喚
	}
}

void EnemyManager::Spawn() {
	int sp;
	while (true) {
		// スポーンさせる場所をランダム選択
		sp = GenerateRandamNum<int>(0, GetSpawnPointSize() - 1);
		// そこにスポーン済みでなければ終了
		if (!spawnPoint_[wave_][sp].enemy) { break; }
	}

	// 敵の初期設定
	Enemy* e = new Enemy();
	e->Init(spawnPoint_[wave_][sp].curve, player_);
	spawnPoint_[wave_][sp].enemy = e;	// 配列に格納
	spawnedEnemyCount_++;	// スポーン済みカウント+1
}

void EnemyManager::StartWave(int num) {
	killCount_ = 0;	// キルカウント初期化
	wave_ = num;	// ウェーブを登録
	assert(wave_ < kMaxWave);	// ウェーブが最大値より大きい場合はエラー
}

void EnemyManager::DebugGUI() {
	static int waveIndex = 0;
	static int enemyIndex = 0;

	ImGui::Begin("EnemyManager");
	ImGui::Text("Quota : %d / %d", killCount_, quota_);
	ImGui::Text("--------------------");
	ImGui::SliderInt("WaveIndex", &waveIndex, 0, kMaxWave - 1);
	int enemySize = static_cast<int>(spawnPoint_[waveIndex].size()) - 1;
	ImGui::SliderInt("EnemyIndex", &enemyIndex, 0, enemySize);
	if (enemyIndex > enemySize) { enemyIndex = enemySize; }	// 配列外参照回避
	// 存在があれば表示
	if (spawnPoint_[waveIndex][enemyIndex].enemy) { spawnPoint_[waveIndex][enemyIndex].enemy->DebugGUI(); }
	else { ImGui::Text("Not Spawned"); }
	ImGui::End();
}