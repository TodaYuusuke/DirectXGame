#pragma once
#include <Adapter.h>

/// <summary>
/// 地面で跳ねる破片
/// </summary>
class TerrainParticle : public LWP::Object::Particle {
private: // ** メンバ変数 ** //

	// 重力
	float kGravity = -9.8f / 1500.0f;
	// 消滅までの時間
	float kDeleteTime = 4.0f;


private: // ** 純粋仮想関数のオーバーライド ** //

	/// <summary>
	/// パーティクルを１つ生成する度に呼ばれる関数
	/// </summary>
	/// <param name="newData">生成された実体の参照（mに基準となるmodelは既にコピーされている）</param>
	/// <returns></returns>
	void Generate(Data& newData) override;

	/// <summary>
	/// パーティクルの更新処理
	/// </summary>
	/// <param name="data">更新する実態の参照</param>
	/// <returns></returns>
	bool UpdateParticle(Data& data) override;
};