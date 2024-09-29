#include "Bullet.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Info;

Bullet::Bullet(LWP::Math::Vector3 position, LWP::Math::Vector3 direction, LWP::Object::Terrain* terrain, TerrainBulletParticle* particle) : capsule_(collider_.SetBroadShape(Collider::Capsule())) {
	// モデル初期設定
	model_.LoadSphere();
	model_.worldTF.translation = position;
	model_.worldTF.scale = { 0.1f,0.1f,0.1f };

	// コライダー
	//collider_.SetFollowTarget(&model_.worldTF);
	collider_.mask.SetBelongFrag(lwpC::Collider::Bullet);	// フラグ設定
	collider_.mask.SetHitFrag(lwpC::Collider::Enemy);
	capsule_.start = model_.worldTF.GetWorldPosition();
	capsule_.end = capsule_.start;
	capsule_.radius = 0.2f;

	// 地形とのコライダー
	terrainPoint_.Init(terrain, &model_.worldTF);

	// 引数を保持
	direction_ = direction;
	particle_ = particle;
}

void Bullet::Update() {
	if (!isAlive_) { return; }

	// 地形にヒットしていたなら終了
	if (terrainPoint_.preFrameHit) {
		// ここでパーティクルを生成する場所を計算
		Vector3 start = capsule_.start;
		Vector3 end = capsule_.end;
		float modelY = model_.worldTF.GetWorldPosition().y;

		// パラメータtを計算
		float t = (modelY - start.y) / (end.y - start.y);

		// tを使ってX, Z座標を計算
		Vector3 result;
		result.x = start.x + t * (end.x - start.x);
		result.y = modelY;
		result.z = start.z + t * (end.z - start.z);
		
		// ここでパーティクル生成
		particle_->Add(10, result);

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

	// 前回の終点を始点に
	capsule_.start = capsule_.end;
	// 速度を座標に加算
	model_.worldTF.translation += direction_ * kSpeed_;
	// 終点を更新
	capsule_.end = model_.worldTF.GetWorldPosition();
}