#pragma once
#include <Adapter.h>

/// <summary>
/// 地面に当たった弾から出す破片
/// </summary>
class TerrainBulletParticle : public LWP::Object::Particle<LWP::Resource::RigidModel> {
public: // ** メンバ変数 ** //
	// 地形のポインタ
	LWP::Object::Terrain* terrain;
	// 識別番号、地形との当たり判定のマップ
	std::map<int, LWP::Object::TerrainCollider::Point> map;

private: // ** 純粋仮想関数のオーバーライド ** //

	/// <summary>
	/// パーティクルを１つ生成する度に呼ばれる関数
	/// </summary>
	/// <param name="newData">生成された実体の参照（mに基準となるmodelは既にコピーされている）</param>
	/// <returns></returns>
	void Generate(Data& newData) override {
		// 大きさをランダムにする
		int scale = LWP::Utility::GenerateRandamNum<int>(90, 110);// 既にコピー済みなので意味なし
		newData.m.worldTF.scale *= static_cast<float>(scale) / 100.0f;

		// 速度ベクトルを生成
		int dir1 = LWP::Utility::GenerateRandamNum<int>(-25, 25);
		int dir2 = LWP::Utility::GenerateRandamNum<int>(0, 100);
		int dir3 = LWP::Utility::GenerateRandamNum<int>(-25, 25);
		// 発射のベクトル
		LWP::Math::Vector3 dir{ dir1 / 100.0f,dir2 / 100.0f, dir3 / 100.0f };
		float multiply = LWP::Utility::GenerateRandamNum<int>(20, 50) / 100.0f;
		newData.velocity = dir.Normalize() * multiply;

		// 地形との当たり判定を生成
		map[newData.idNumber].Init(terrain, &newData.m.worldTF);
	};
	/// <summary>
	/// パーティクルの更新処理
	/// </summary>
	/// <param name="data">更新する実態の参照</param>
	/// <returns></returns>
	bool UpdateParticle(Data& data) override {
		// 速度がある程度ある間の処理
		if (data.velocity.Length() > 0.01f) {
			// 前フレームに地形と当たっていたなら -> 跳ねる
			if (map[data.idNumber].preFrameHit) {
				data.velocity.y *= -0.35f;
				data.velocity.x *= 0.6f;
				data.velocity.z *= 0.6f;
			}

			data.m.worldTF.translation += data.velocity;    // 速度ベクトルを加算
			data.m.worldTF.rotation += data.velocity;    // ついでに回転させとく
			data.m.worldTF.rotation = data.m.worldTF.rotation.Normalize();

			// 重力を加算
			data.velocity.y += -9.8f / 600.0f;
		}
		// 速度がなくなった時の処理
		else if(data.elapsedFrame <= 60) {
			data.velocity = { 0.0f,0.0f,0.0f };

			// 経過フレーム加算
			data.elapsedFrame++;
			data.m.worldTF.scale = LWP::Utility::Interp::Lerp({ 0.4f,0.4f,0.4f }, { 0.0f,0.0f,0.0f }, LWP::Utility::Easing::InCubic(float(data.elapsedFrame) / 60.0f));
		}
		// 1秒かけて消滅アニメーションを完了したとき
		else {
			map.erase(data.idNumber);	// 地形のコライダー削除
			return true;
		}

		return false;
	};
};