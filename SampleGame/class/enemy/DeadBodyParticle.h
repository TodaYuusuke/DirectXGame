#pragma once
#include "BloodParticle.h"

/// <summary>
/// 死体が吹き飛ぶパーティクル
/// </summary>
class DeadBodyParticle : public BloodParticle {
private: // ** 純粋仮想関数のオーバーライド ** //

	/// <summary>
	/// パーティクルを１つ生成する度に呼ばれる関数
	/// </summary>
	/// <param name="newData">生成された実体の参照（mに基準となるmodelは既にコピーされている）</param>
	/// <returns></returns>
	void Generate(Data& newData) override {
		BloodParticle::Generate(newData);
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
			col;
		};

		// 消滅までの時間を調整
		kDeleteTime = 4.0f;
	};
};
