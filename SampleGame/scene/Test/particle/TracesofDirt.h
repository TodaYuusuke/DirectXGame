#pragma once
#include <Adapter.h>

/// <summary>
/// 土埃
/// </summary>
class TracesofDirt : public LWP::Object::SpriteParticle {
public: // ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	TracesofDirt();

private: // ** メンバ変数 ** //

	// 生成の範囲
	LWP::Math::Vector2 kGenerateMinPoint = { 100.0f, 100.0f, };
	LWP::Math::Vector2 kGenerateMaxPoint = { 300.0f, 100.0f, };
	float kGenerateRange = 25.0f;
	// 消滅までの時間
	float kDeleteTime = 5.0f;


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