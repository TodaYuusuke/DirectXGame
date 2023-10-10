#pragma once
#include "Enemy.h"
#include <vector>

class EnemyManager final {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyManager() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyManager() = default;


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="playerPos">追従するプレイヤーのトランスフォーム</param>
	void Initialize(LWP::Object::WorldTransform* playerPos);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	// ** 外部から操作するための関数 ** //
	

	/// <summary>
	/// 敵を追加する関数
	/// </summary>
	/// <param name="addAmount">追加する数</param>
	void AddEnemy(int addAmount);


private: // ** メンバ変数 ** //

	// マップサイズ
	const double kMapSize_ = 2.0f;

	// エネミーの可変長配列
	std::vector<Enemy> enemys_;

	// プレイヤーのトランスフォームポインタ
	LWP::Object::WorldTransform* playerPos_;
};