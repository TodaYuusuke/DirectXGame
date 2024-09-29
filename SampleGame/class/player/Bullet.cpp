#include "Bullet.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Info;

Bullet::Bullet(LWP::Math::Vector3 position, LWP::Math::Vector3 direction, LWP::Object::Terrain* terrain) : capsule_(collider_.SetBroadShape(Collider::Capsule())) {
	// モデル初期設定
	model_.LoadSphere();
	model_.worldTF.translation = position;
	model_.worldTF.scale = { 0.1f,0.1f,0.1f };

	// コライダー
	capsule_.SetFollowPtr(&model_.worldTF);
	capsule_.radius = 0.2f;

	// 地形とのコライダー
	terrainPoint_.Init(terrain, &model_.worldTF);

	// 引数を保持
	direction_ = direction;
}

void Bullet::Update() {
	if (!isAlive_) { return; }

	// 地形にヒットしていたなら終了
	if (terrainPoint_.preFrameHit) {
		// ここでパーティクル生成
		isAlive_ = false;
		terrainPoint_.wtf = nullptr;	// 追従対象をnullにして解放
		return;
	}
	// 経過時間更新
	time_ += GetDeltaTimeF();
	// 数秒経過していたら終了
	if (time_ > kAliveTime_) {
		isAlive_ = false;
		terrainPoint_.wtf = nullptr;	// 追従対象をnullにして解放
		return;
	}

	// 前回の地点
	Vector3 prePos = model_.worldTF.GetWorldPosition();
	// 速度を座標に加算
	model_.worldTF.translation += direction_ * kSpeed_;
	// 今回の地点
	Vector3 currentPos = model_.worldTF.GetWorldPosition();

	// カプセルの終点を設定
	capsule_.end = prePos - currentPos;
}