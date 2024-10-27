#pragma once
#include <Adapter.h>

/// <summary>
/// 弾を食らって血が飛び散るパーティクル
/// </summary>
class BloodParticle : public LWP::Object::Particle {
protected: // ** メンバ変数 ** //

	// 重力
	float kGravity_ = -9.8f / 1500.0f;
	// 消滅までの時間
	float kDeleteTime = 2.0f;


protected: // ** 純粋仮想関数のオーバーライド ** //

	/// <summary>
	/// パーティクルを１つ生成する度に呼ばれる関数
	/// </summary>
	/// <param name="newData">生成された実体の参照（mに基準となるmodelは既にコピーされている）</param>
	/// <returns></returns>
	void Generate(Data& newData) override {
		newData.m.materials = model.materials;

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
#if DEMO
		LWP::Object::Collider::Point& p = newData.collider->SetBroadShape(LWP::Object::Collider::Point());
		p.isShowWireFrame = false;	// Debugがおもすぎるので非表示に
#endif
		newData.collider->SetFollowTarget(&newData.m.worldTF);
		newData.collider->mask.SetBelongFrag(lwpC::Collider::Particle);	// フラグ設定
		newData.collider->mask.SetHitFrag(lwpC::Collider::FieldObject | lwpC::Collider::Terrain);
		newData.collider->isMove = true;

		newData.collider->enterLambda = [&newData, this](LWP::Object::Collision* col) {
			// ぶつかった地点で止まる
			newData.velocity = { 0.0f,0.0f,0.0f };
			col;
			newData.collider->isActive = false;	// 当たったらもう当たり判定は使わないのでfalseに
		};
	};
	/// <summary>
	/// パーティクルの更新処理
	/// </summary>
	/// <param name="data">更新する実態の参照</param>
	/// <returns></returns>
	bool UpdateParticle(Data& data) override {
		// 通常の処理
		if (data.collider->isActive) {
			// 重力を加算
			data.velocity.y += kGravity_;

			// 速度ベクトルを加算
			data.m.worldTF.translation += data.velocity;
			data.m.worldTF.rotation += data.velocity;    // ついでに回転させとく	
			data.m.worldTF.rotation = data.m.worldTF.rotation.Normalize();

			// 速度を弱める
			data.velocity *= 0.99f;
		}
		else {
			// 重力を弱めて加算
			data.velocity.y += kGravity_ * 0.2f;
		}

		// 経過フレーム加算
		data.elapsedTime += LWP::Info::GetDeltaTimeF();
		if (data.elapsedTime > kDeleteTime) { data.elapsedTime = kDeleteTime; }

		data.m.worldTF.scale = LWP::Utility::Interp::Lerp(model.worldTF.scale, { 0.0f,0.0f,0.0f }, LWP::Utility::Easing::InCubic(data.elapsedTime / kDeleteTime));
		// 1秒かけて消滅アニメーションを完了したとき消滅する
		if (data.elapsedTime >= kDeleteTime) {
			return true;
		}

		return false;
	};
};
