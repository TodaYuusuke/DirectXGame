#pragma once
#include <Adapter.h>

class Bullet final {
public: // ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Bullet(LWP::Math::Vector3 position, LWP::Math::Vector3 direction, LWP::Object::Terrain* terrain);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 生存フラグを取得
	/// </summary>
	bool GetAlive() { return isAlive_; }


private: // ** メンバ変数 ** //

	// モデル
	LWP::Resource::RigidModel model_;
	// コライダー
	LWP::Object::Collider::Collider collider_;
	LWP::Object::Collider::Capsule& capsule_;

	// 地形とのコライダーポインタ
	LWP::Object::TerrainCollider::Point terrainPoint_;
	
	// 経過時間
	float time_ = 0.0f;
	// 進む方向ベクトル
	LWP::Math::Vector3 direction_;
	// 弾の生存フラグ
	bool isAlive_ = true;

	// 弾の速度
	float kSpeed_ = 10.0f;
	// 消滅までの時間
	float kAliveTime_ = 3.0f;
};