#include "EnemyManager.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Object;
using namespace LWP::Resource;
using namespace LWP::Utility;
using namespace LWP::Info;

EnemyManager::EnemyManager() : PBlood_(100) {

}
void EnemyManager::Init(LevelData* level, Player* player) {
	// ポインタを保持
	player_ = player;

	// パーティクル初期設定
	PBlood_.model.LoadCube();
	PBlood_.SetShaderPath("Blood/Emitter.CS.hlsl", "Blood/Update.CS.hlsl", "Blood/Hit.CS.hlsl");
	PDeadBody_.model.LoadCube();
	PDeadBody_.model.worldTF.scale = { 0.1f, 0.1f ,0.1f };
	PDeadBody_.model.materials["Material"].color = Color(0.463f, 0.592f, 0.318f, 1.0f);
	
	// ウェーブデータ設定
	waveData_[0].kQuota = 15;
	waveData_[0].kSpeedMultiply = (1.0f / 12.0f);
	waveData_[1].kQuota = 20;
	waveData_[1].kSpeedMultiply = (1.0f / 5.0f);

	// 敵のスポーン地点を保存
	for (int i = 0; i < Wave::kMax; i++) {
		for (int j = 0; ; j++) {
			std::ostringstream oss;
			oss << std::setw(3) << std::setfill('0') << j;	// 3桁の0埋めの数字
			std::string str = "EnemySP" + std::to_string(i) + "." + oss.str();	// 参照する名前を決定
			// その名前が存在するか確認
			if (level->catmullRomCurves.contains(str)) {
				spawnPoint_[i].push_back(&level->catmullRomCurves[str]);	// 存在したので保存
			}
			else {
				break;	// 存在しないので次のウェーブへ
			}
		}
	}
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
	if (spawnedEnemyCount_ + killCount_ >= waveData_[wave_].kQuota) { return; }	// 処理終了


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
	Enemy* e = new Enemy(waveData_[wave_].kSpeedMultiply);
	e->Init(spawnPoint_[wave_][sp].curve, player_,
		[this](Vector3 pos) {
			PBlood_.Add(10, (pos));
			PDeadBody_.Add(kParticleSize_, (pos));
		}
	);	// 死亡時のパーティクルを生成する関数を渡す
	spawnPoint_[wave_][sp].enemy = e;	// 配列に格納
	spawnedEnemyCount_++;	// スポーン済みカウント+1
}

void EnemyManager::StartWave(int num) {
	killCount_ = 0;	// キルカウント初期化
	wave_ = num;	// ウェーブを登録
	assert(wave_ < Wave::kMax);	// ウェーブが最大値より大きい場合はエラー
}

void EnemyManager::DebugGUI() {
	static int waveIndex = 0;
	static int enemyIndex = 0;

	ImGui::Begin("EnemyManager");
	if (wave_ != -1) { ImGui::Text("Quota : %d / %d", killCount_, waveData_[wave_].kQuota); }
	ImGui::Text("--------------------");
	ImGui::SliderInt("WaveIndex", &waveIndex, 0, Wave::kMax - 1);
	int enemySize = static_cast<int>(spawnPoint_[waveIndex].size()) - 1;
	ImGui::SliderInt("EnemyIndex", &enemyIndex, 0, enemySize);
	if (enemyIndex > enemySize) { enemyIndex = enemySize; }	// 配列外参照回避
	// 存在があれば表示
	if (spawnPoint_[waveIndex][enemyIndex].enemy) { spawnPoint_[waveIndex][enemyIndex].enemy->DebugGUI(); }
	else { ImGui::Text("Not Spawned"); }
	ImGui::End();
}