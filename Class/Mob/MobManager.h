#pragma once
#include "Derived/DefenseObject.h"
#include "Derived/Enemy.h"
#include <list>

#include "../Map/MapManager.h"

class MobManager final {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update(MapManager* mapManager);


	// ** 外部から操作するための関数 ** //

	/// <summary>
	/// 敵を追加する関数
	/// </summary>
	/// <param name="addAmount">追加する数</param>
	void AddEnemy(int addAmount);

	/// <summary>
	/// 防衛対象が死亡したかをチェックする関数
	/// </summary>
	bool GetTargetDead() { return !target_.isActive_; }
	
public : // ** メンバ変数 ** //

	// スコア
	int32_t score_;

private: // ** メンバ変数 ** //
	
	// 防衛対象
	DefenseObject target_;
	// エネミーの可変長配列
	std::list<Enemy> enemies_;

};