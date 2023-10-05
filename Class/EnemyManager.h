#pragma once
#include "Enemy.h"
#include <vector>

class EnemyManager final {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	EnemyManager();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyManager();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


private: // ** メンバ変数 ** //

	std::vector<Enemy> enemys_;
};