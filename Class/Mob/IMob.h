#pragma once
#include <Adapter.h>

class IMob {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	
	/// <summary>
	/// トランスフォームを受け取る関数
	/// </summary>
	LWP::Object::WorldTransform GetTransform() { return model_->transform; }

protected: // ** プライベートな関数 ** //

	/// <summary>
	/// 地上にいる際の処理
	/// </summary>
	virtual void Moving();
	/// <summary>
	/// 空中にいる際の処理
	/// </summary>
	void Flying();

	/// <summary>
	/// Y軸を最高速度を受け取る
	/// </summary>
	virtual float GetMaxSpeed() { return 0.002f; }

	/// <summary>
	/// 反発係数を受け取る
	/// </summary>
	virtual float GetBounceFactor() { return 0.4f; }

	/// <summary>
	/// ダメージの係数を受け取る
	/// </summary>
	virtual float GetDamageMultiply() { return 2.5f; }

protected: // ** メンバ定数 ** //

	// 重力加速度
	const float kGravityAccele = 9.8f / 60.0f / 60.0f;
	// 限界速度
	const float kLimitSpeed = 0.03f;

protected: // ** メンバ変数 ** //

	// ** リソース関連 ** //

public:
	// モデル
	LWP::Primitive::Mesh* model_ = nullptr;
	// サイズの基準値
	float Size = 0.5f;

	// ** パラメータ ** //

	// 速度
	LWP::Math::Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
	// HP
	int hp_ = 100;

	// ** フラグ関連 ** //

	// 生存フラグ( true == 生存 )
	bool isActive_ = true;
	// 空中フラグ
	bool isFlying_ = false;

};