#pragma once
#include <scene/IScene.h>
#include <Adapter.h>

#include <sstream>

#include "../GameObject/RailCamera/RailCamera.h"

#include "../GameObject/Player/Player.h"
#include "../GameObject/Enemy/Enemy.h"
#include "../GameObject/Skydome/Skydome.h"
#include "../GameObject/Collision/CollisionManager.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene :
	public IScene {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 敵を追加する
	/// </summary>
	/// <param name="enemy">初期座標</param>
	void AddEnemy(LWP::Math::Vector3 position);

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	/// <param name="enemyBullet">敵弾</param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();


private: // メンバ変数
	// レールカメラ
	RailCamera* railCamera_ = nullptr;

	// 当たり判定マネージャー
	CollisionManager* collisionManager_ = nullptr;
	// 天球
	Skydome* skydome_ = nullptr;

	// プレイヤー
	Player* player_ = nullptr;
	// 敵のリスト
	std::list<Enemy*> enemys_;
	// 敵弾のリスト
	std::list<EnemyBullet*> enemyBullets_;

	// ファイル読み取り
	std::stringstream enemyPopCommands;
	// 待機中フラグ
	bool isWaiting = false;
	// 待機中タイマー
	int waitTimer = 0;

private: // メンバ関数
};
