#include "CharacterManager.h"

CharacterManager::~CharacterManager() {
	//for (Enemy* enemy : enemies_) {
	//	delete enemy;
	//}
}


void CharacterManager::Initialize() {
	// プレイヤーの生成
	player_ = std::make_unique<Player>();
	player_->Initialize({0.0f, 4.0f, 0.0f});

	enemies_.clear();

	SetEnemyMovement();

	//// 当たり判定マネージャーを生成
	collisionManager_ = std::make_unique<CollisionManager>();

	killedEnemyCount_ = 0;

	EffectManager::GetInstance()->Initialize();
}
void CharacterManager::Update() {
	collisionManager_->ClearList();	// 当たり判定初期化

	// 雑魚敵を召喚
	if (enemySpawnFrame_ < 0) {
		SummonEnemies(static_cast<EnemyMoveType>(std::rand() % EnemyMoveTypeSize));
		enemySpawnFrame_ = kEnemySpawnCycle_;
	} else {
		enemySpawnFrame_--;
	}
	// コールバック更新
	for (TimedCall<void()>* timedCall : timedCalls_) {
		timedCall->Update();
	}

	// 更新処理
	player_->Update();
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// デスフラグの立った敵を削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->GetisDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 当たり判定を登録
	collisionManager_->PushCollider(player_.get());
	// 敵の全ての当たり判定
	for (const auto& enemy : enemies_) {
		collisionManager_->PushCollider(enemy);
	}
	// 弾リストの取得
	const std::list<Bullet*>& Bullets = player_->GetBullets();
	// 弾の全ての当たり判定
	for (Bullet* bullet : Bullets) {
		collisionManager_->PushCollider(bullet);
	}

	collisionManager_->CheckAllCollisions();

	EffectManager::GetInstance()->Update();
}

void CharacterManager::SetEnemyMovement() {
	// 雑魚の挙動を定義
	enemyControlPoints_[Parallel1] = {
	    {-70, -60, 70},
        {-60, 8,  70},
        {0,   10, 70},
        {60,  8,  70},
        {70,  -60, 70}
    };
	enemyControlPoints_[Parallel2] = {
	    {70, -60, 70},
        {60, 8,  70},
        {0,   10, 70},
        {-60,  8,  70},
        {-70,  -60, 70}
    };
	enemyControlPoints_[Parallel3] = {
	    {70,  -60, -70},
        {60,  8,  -70},
        {0,   10, -70},
        {-60, 8,  -70},
        {-70, -60, -70}
    };
	enemyControlPoints_[Parallel4] = {
	    {-70,  -60, -70},
        {-60,  8,  -70},
        {0,   10, -70},
        {60, 8,  -70},
        {70, -60, -70}
    };
	enemyControlPoints_[Parallel5] = {
	    {70, -60, -70},
        {70, 8,  -60},
        {70, 10, 0  },
        {70, 8,  60 },
        {70, -60, 70 }
    };
	enemyControlPoints_[Parallel6] = {
	    {70, -60, 70},
        {70, 8,  60},
        {70, 10, 0},
        {70, 8,  -60 },
        {70, -60, -70 }
    };
	enemyControlPoints_[Parallel7] = {
	    {-70, -60, 70 },
        {-70, 8,  60 },
        {-70, 10, 0  },
        {-70, 8,  -60},
        {-70, -60, -70}
    };
	enemyControlPoints_[Parallel8] = {
	    {-70, -60, -70 },
        {-70, 8,  -60 },
        {-70, 10, 0  },
        {-70, 8,  60},
        {-70, -60, 70}
    };
}
void CharacterManager::SummonEnemy(EnemyMoveType moveType) {
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize({30.0f, -50.0f, 0.0f});
	newEnemy->SetTextureHandle(enemyTextureHandle_);
	newEnemy->SetPlayerWorldTransform(player_->GetTransform());
	newEnemy->SetControlPoints(enemyControlPoints_[moveType]);
	newEnemy->killCount_ = &killedEnemyCount_;
	enemies_.push_back(newEnemy);
}
void CharacterManager::SummonEnemies(EnemyMoveType moveType) {
	std::function<void()> f = std::bind(&CharacterManager::SummonEnemy, this, moveType);
	SummonEnemy(moveType);
	timedCalls_.push_back(new TimedCall<void()>(f, 90));
	timedCalls_.push_back(new TimedCall<void()>(f, 180));
}
