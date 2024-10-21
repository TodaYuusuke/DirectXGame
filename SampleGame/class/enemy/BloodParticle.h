#pragma once
#include <Adapter.h>

/// <summary>
/// 弾を食らって血が飛び散るパーティクル
/// </summary>
class BloodParticle : public LWP::Object::Particle<LWP::Resource::RigidModel> {
public: // ** パブリックなメンバ変数 ** //
	
	// 被弾した弾の発射元
	LWP::Math::Vector3 bulletPos;


private: // ** 純粋仮想関数のオーバーライド ** //

	/// <summary>
	/// パーティクルを１つ生成する度に呼ばれる関数
	/// </summary>
	/// <param name="newData">生成された実体の参照（mに基準となるmodelは既にコピーされている）</param>
	/// <returns></returns>
	void Generate(Data& newData) override {
		newData.m.materials = model.materials;
		// 発射のベクトル
		LWP::Math::Vector3 dir = LWP::Math::Vector3(model.worldTF.GetWorldPosition() - bulletPos).Normalize();
		// XとYの角度を調整
		dir.x += LWP::Utility::GenerateRandamNum<int>(-100, 100) / 1000.0f;
		dir.y += LWP::Utility::GenerateRandamNum<int>(-100, 100) / 1000.0f;
		newData.velocity = dir.Normalize() * 0.2f;

		// 地形との当たり判定を生成
		newData.collider = new LWP::Object::Collision;
		newData.collider->SetFollowTarget(&newData.m.worldTF);
		newData.collider->mask.SetBelongFrag(lwpC::Collider::Particle);	// フラグ設定
		newData.collider->mask.SetHitFrag(lwpC::Collider::FieldObject | lwpC::Collider::Terrain);
		newData.collider->isMove = true;
		newData.collider->enterLambda = [&newData, this](LWP::Object::Collision* col) {
			// ぶつかった地点で止まる
			newData.velocity = { 0.0f,0.0f,0.0f };
			col;
		};
	};
	/// <summary>
	/// パーティクルの更新処理
	/// </summary>
	/// <param name="data">更新する実態の参照</param>
	/// <returns></returns>
	bool UpdateParticle(Data& data) override {
		// 速度がある間の処理
		if (data.velocity.Length() > 0.0f) {
			data.m.worldTF.translation += data.velocity;    // 速度ベクトルを加算
			data.m.worldTF.translation.y += -9.8f / 140.0f;    // 重力を加算
		}
		// 着弾後の処理
		else {
			// 経過フレーム加算
			data.elapsedTime += LWP::Info::GetDeltaTimeF();
			if (data.elapsedTime > 1.0f) {
				data.elapsedTime = 1.0f;
			}

			data.m.worldTF.scale = LWP::Utility::Interp::Lerp(model.worldTF.scale, { 0.0f,0.0f,0.0f }, LWP::Utility::Easing::InCubic(data.elapsedTime));
			// 1秒かけて消滅アニメーションを完了したとき消滅する
			if (data.elapsedTime >= 1.0f) {
				return true;
			}
		}

		return false;
	};
};
