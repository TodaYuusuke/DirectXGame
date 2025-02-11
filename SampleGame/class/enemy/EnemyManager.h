#pragma once
#include "Enemy.h"

#include "BloodParticle.h"
#include "DeadBodyParticle.h"

/// <summary>
/// 敵出現を管理するクラス
/// </summary>
class EnemyManager final {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyManager();
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(LWP::Resource::LevelData* level, Player* player);
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ウェーブ開始
	/// </summary>
	/// <param name="num"></param>
	void StartWave(int num);
	/// <summary>
	/// ノルマ分敵を殺したら終了フラグを返す
	/// </summary>
	bool GetClearFlag() { return killCount_ >= quota_; }

	/// <summary>
	/// デバッグ用のImGui
	/// </summary>
	void DebugGUI();


private: // ** メンバ変数 ** //
	// プレイヤーのポインタ
	Player* player_;

	// ウェーブ
	struct Wave {
		// 静的定数
		const static int kMax = 2;

		// ノルマ数
		int kQuota;
		// 敵の速度
		float kSpeedMultiply;
	};
	Wave waveData_[Wave::kMax];
	// ウェーブ番号
	int wave_ = -1;

	// 敵の出現ポイントと敵クラス
	struct SpawnPoint {
		LWP::Utility::CatmullRom* curve = nullptr;
		Enemy* enemy = nullptr;

		// コンストラクタ
		SpawnPoint(LWP::Utility::CatmullRom* ptr) { curve = ptr; }
		// デストラクタ
		~SpawnPoint() {
			if (curve) { curve = nullptr; }	// 消すとLevelData内のデータを消してしまうのでnull代入
			if (enemy) { delete enemy; }
		}
	};
	std::vector<SpawnPoint> spawnPoint_[Wave::kMax];


	// 敵出現間隔
	float kSummonInterval_ = 1.5f;
	float summonInterval_ = 0.0f;
	
	// 現在の敵出現数
	int spawnedEnemyCount_ = 0;
	// 敵撃破数
	int killCount_ = 0;
	// 敵撃破ノルマ
	int quota_ = 15;

	// パーティクルクラス
	LWP::Object::GPUParticle PBlood_;
	DeadBodyParticle PDeadBody_;
	// パーティクルの数
	int kParticleSize_ = 32 * 4;

private: // ** メンバ関数 ** //
	
	/// <summary>
	/// 召喚処理
	/// </summary>
	void Spawn();

	/// <summary>
	/// ウェーブ内の最大スポーン数を取得
	/// </summary>
	/// <returns></returns>
	int GetSpawnPointSize() { return static_cast<int>(spawnPoint_[wave_].size()); }
};