#pragma once
#include <Adapter.h>

/// <summary>
/// パーティクル
/// </summary>
class DamageParticle : public LWP::Object::Particle<LWP::Resource::RigidModel> {
private: // ** 純粋仮想関数のオーバーライド ** //

	/// <summary>
	/// パーティクルを１つ生成する度に呼ばれる関数
	/// </summary>
	/// <param name="newData">生成された実体の参照（mに基準となるmodelは既にコピーされている）</param>
	/// <returns></returns>
	void Generate(Data& newData) override {
		newData.m.worldTF.translation = lwp::Vector3{ 0.0f,1.0f,0.0f } + model.worldTF.GetWorldPosition();
		//newData.m.worldTF.rotation = model.worldTF.rotation;	// 既にコピー済みなので意味なし
		newData.m.worldTF.scale = { 0.15f,0.15f, 0.15f };

		// 速度ベクトルを生成
		int dir1 = LWP::Utility::GenerateRandamNum<int>(-200, 200);
		int dir2 = LWP::Utility::GenerateRandamNum<int>(-200, 200);
		int dir3 = LWP::Utility::GenerateRandamNum<int>(-200, 200);
		// 発射のベクトル
		LWP::Math::Vector3 dir{ dir1 / 100.0f,dir2 / 100.0f, dir3 / 100.0f };
		float multiply = LWP::Utility::GenerateRandamNum<int>(10, 30) / 100.0f;
		newData.velocity = dir.Normalize() * multiply;
	};
	/// <summary>
	/// パーティクルの更新処理
	/// </summary>
	/// <param name="data">更新する実態の参照</param>
	/// <returns></returns>
	bool UpdateParticle(Data& data) override {
		if (LWP::Info::GetDeltaTime() == 0.0f) { return false; }

		// 経過フレーム追加
		data.elapsedFrame++;

		data.m.worldTF.translation += data.velocity;    // 速度ベクトルを加算
		data.m.worldTF.translation.y += -9.8f / 80.0f;    // 重力を加算

		// 速度ベクトルを弱める
		data.velocity *= 0.9f;

		// 地面についたら薄く広がる
		if (data.m.worldTF.translation.y <= 0.1f) {
			// 座標固定
			data.m.worldTF.translation.y = 0.1f;
			// 速度をなくす
			data.velocity = { 0,0,0 };
			// x,z方向に伸ばす
			data.m.worldTF.scale.x += 0.001f;
			data.m.worldTF.scale.z += 0.001f;
		}
		else {
			// 速度ベクトルを弱める
			data.velocity *= 0.9f;
		}

		return data.elapsedFrame > 50 ? true : false;
	};
};
