#include "GameScene.h"
#include <cassert>
#include <fstream>

using namespace LWP::Math;
using namespace LWP::Input;

GameScene::~GameScene() {
	delete player_;
	delete skydome_;
	delete railCamera_;
	delete collisionManager_;
	delete player_;
	for (Enemy* enemy : enemys_) {
		delete enemy;
	}
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		delete enemyBullet;
	}
}

void GameScene::Initialize() {
	// レールカメラの生成
	railCamera_ = new RailCamera();
	// レールカメラの初期化
	railCamera_->Initialize({0, 0, -30}, {0, 0, 0});

	// 当たり判定マネージャーの生成
	collisionManager_ = new CollisionManager();
	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(railCamera_->GetCamera());

	// 弾初期化
	enemyBullets_.clear();

	// 敵の初期化と生成
	enemys_.clear();

	// ファイル読み込み
	LoadEnemyPopData();

	// カメラをメイン描画に
	SetMainRenderCamera(railCamera_->GetCamera());
	//mainCamera->transform.translation.z = -50.0f;
	// フルスクリーンに
	LWP::Window::ChangeFullScreenMode();

}

void GameScene::Update() {
	// ファイル読み込み更新
	UpdateEnemyPopCommands();

	// 当たり判定マネージャーの初期化
	collisionManager_->ClearList();

	// リトライ
	if (Keyboard::GetTrigger(DIK_R)) {
		nextScene_ = new GameScene();
	}
	// プログラム終了
	if (Keyboard::GetTrigger(DIK_ESCAPE)) {
		LWP::System::End();
	}

	// レールカメラ更新
	railCamera_->Update();

	// 自キャラの更新
	player_->Update();
	//OutputDebugStringA(("x:" + std::to_string(player_->GetWorldPosition().x) + " y:" + std::to_string(player_->GetWorldPosition().y) + " z:" + std::to_string(player_->GetWorldPosition().z) + "\n").c_str());

	// 敵の更新
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}
	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (!enemy->isAlive()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 敵弾の更新
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		enemyBullet->Update();
	}
	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (!bullet->isAlive()) {
			delete bullet;
			return true;
		}
		return false;
	});


	// 当たり判定を登録
	collisionManager_->PushCollider(player_);
	// 弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 自弾の全ての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		collisionManager_->PushCollider(bullet);
	}
	// 敵の全ての当たり判定
	for (Enemy* enemy : enemys_) {
		collisionManager_->PushCollider(enemy);
	}
	// 敵弾の全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets_) {
		collisionManager_->PushCollider(bullet);
	}

	collisionManager_->CheckAllCollisions();
}

void GameScene::AddEnemy(Vector3 position) {
	Enemy* newEnemy = new Enemy();

	newEnemy->SetPlayer(player_);
	newEnemy->SetGameScene(this);
	newEnemy->Initialize(position);

	enemys_.push_back(newEnemy);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("./resources/csv/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列をストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	// 待機処理
	if (isWaiting) {
		waitTimer--;
		if (waitTimer <= 0) {
			// 待機完了
			isWaiting = false;
		}
		return;
	}
	
	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		else if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word,',');
			float x = static_cast<float>(std::atof(word.c_str()));
			// y座標
			getline(line_stream, word, ',');
			float y = static_cast<float>(std::atof(word.c_str()));
			// z座標
			getline(line_stream, word, ',');
			float z = static_cast<float>(std::atof(word.c_str()));

			// 敵を発生させる
			AddEnemy(Vector3(x, y, z));
		}

		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待機時間
			isWaiting = true;
			waitTimer = atoi(word.c_str());

			// コマンドループを抜ける
			break;
		}
	}
}