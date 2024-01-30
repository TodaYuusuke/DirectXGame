#pragma once
#include "Enemy.h"

// 前方宣言
class Enemy;

class BaseEnemyState {
public: // メンバ関数
	// デストラクタ
	virtual ~BaseEnemyState(){};
	
	// 純粋仮想関数
	virtual void Initialize(Enemy* enemy) = 0;
	virtual void Update(Enemy* enemy) = 0;

protected: // ** メンバ変数 ** //

	// 経過時間
	int elapsedFrame = 0;
};



class EnemyApproach : public BaseEnemyState {

	// 初期化
	void Initialize(Enemy* enemy);
	// 更新処理
	void Update(Enemy* enemy);

	//*　　定　数　　*//

	// 接近　速さ
	const LWP::Math::Vector3 kApproachSpeed = {0, 0, -0.2f};
};

class EnemyLeave : public BaseEnemyState {

	// 初期化
	void Initialize(Enemy* enemy);
	// 更新処理
	void Update(Enemy* enemy);

	
	//*　　定　数　　*//
	
	// 撤退　速さ
	const LWP::Math::Vector3 kLeaveSpeed = {-0.12f, 0.12f, 0};
};