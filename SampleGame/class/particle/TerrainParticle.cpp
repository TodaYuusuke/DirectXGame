#pragma once
#include "TerrainParticle.h"

void TerrainParticle::Generate(Data& newData) {
	// 速度ベクトルを生成
	int dir1 = LWP::Utility::GenerateRandamNum<int>(-100, 100);
	int dir2 = LWP::Utility::GenerateRandamNum<int>(-100, 100);
	int dir3 = LWP::Utility::GenerateRandamNum<int>(-100, 100);
	// 発射のベクトル
	LWP::Math::Vector3 dir{ dir1 / 100.0f,dir2 / 100.0f, dir3 / 100.0f };
	float multiply = LWP::Utility::GenerateRandamNum<int>(20, 50) / 100.0f;
	newData.velocity = dir.Normalize() * multiply;

	// 地形との当たり判定を生成
	newData.collider = new LWP::Object::Collision;
	newData.collider->SetFollowTarget(&newData.m.worldTF);
	newData.collider->mask.SetBelongFrag(lwpC::Collider::Particle);	// フラグ設定
	newData.collider->mask.SetHitFrag(lwpC::Collider::FieldObject | lwpC::Collider::Terrain);
	newData.collider->isMove = true;

	// ヒット時の処理だけ変える
	newData.collider->enterLambda = [&newData, this](LWP::Object::Collision* col) {
		// 地形オブジェクトに当たった場合
		if (col->mask.GetBelongFrag() & lwpC::Collider::FieldObject) {
			// ぶつかったら反転する
			newData.velocity.x *= -0.5f;
			newData.velocity.z *= -0.5f;
		}
		// Terrainに当たった場合
		else if (col->mask.GetBelongFrag() & lwpC::Collider::Terrain) {
			newData.velocity.y *= -0.5f;
			newData.velocity.x *= 0.5f;	// 速度は落とす
			newData.velocity.z *= 0.5f;
		}
	};
}
bool TerrainParticle::UpdateParticle(Data& data) {
	// 重力を加算
	data.velocity.y += kGravity;

	// 速度ベクトルを加算
	data.m.worldTF.translation += data.velocity;
	data.m.worldTF.rotation += data.velocity;    // ついでに回転させとく	
	data.m.worldTF.rotation = data.m.worldTF.rotation.Normalize();

	// 速度を弱める
	data.velocity *= 0.99f;

	// 経過フレーム加算
	data.elapsedTime += LWP::Info::GetDeltaTimeF();
	if (data.elapsedTime > kDeleteTime) { data.elapsedTime = kDeleteTime; }

	data.m.worldTF.scale = LWP::Utility::Interp::Lerp(model.worldTF.scale, { 0.0f,0.0f,0.0f }, LWP::Utility::Easing::InCubic(data.elapsedTime / kDeleteTime));
	// kDeleteTime秒かけて消滅アニメーションを完了したとき消滅する
	if (data.elapsedTime >= kDeleteTime) {
		return true;
	}

	return false;
}