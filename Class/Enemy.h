#pragma once
#include "../Adapter/LWP.h"

// ** 定数 **//

// フィールドの中心座標(Vector2)
const LWP::Math::Vector3 fieldCenterPos = { 0.0f,0.0f,60.0f };
// フィールドの外周までの半径
const float fieldRadius = 12.0f;
// 重力
const float gravity = 9.8f;

class Enemy final {
public:
	// ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy(LWP::Object::WorldTransform* targetPos);
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="targetPos">護衛対象の座標</param>
	void Initialize(LWP::Object::WorldTransform* targetPos);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// ** アクセッサ ( セッタ -> ゲッタ の順で記載) ** //

	/// <summary>
	/// 浮遊させる
	/// </summary>
	void SetFloat();

	/// <summary>
	/// 吹き飛ばす
	/// </summary>
	void SetFly();

	/// <summary>
	/// ダメージを与える
	/// </summary>
	/// <param name="damege">与えるダメージ</param>
	void Damege(int32_t damege);

	/// <summary>
	/// 即死させる
	/// </summary>
	void SetDeath();

	/// <summary>
	/// 敵の生死を確認する
	/// </summary>
	bool GetIsActive();






private: // ** メンバ変数 ** //

	/// ** リソース関連 ** ///

	// 敵のモデル
	LWP::Primitive::Mesh* model_;
	
	//** テスト用 **//
	
	// テスト用の護衛対象のモデル
	LWP::Primitive::Mesh* testTargetModel_;
	// テスト用のフィールドモデル
	LWP::Primitive::Mesh* testFieldModel_;
	

	///** パラメータ **///
	
	// 敵の座標
	LWP::Object::WorldTransform* worldTransform_ = nullptr;
	// 敵の半径
	LWP::Math::Vector3 radius_;
	// 移動量
	LWP::Math::Vector2 velocity_;
	// 敵のHP
	int32_t hp_;
	// 敵の浮遊時間(最大)
	const int32_t floatTimeMax_ = 120;
	// 浮遊時間(現在値)
	int32_t floatTime_;

	///** 外部パラメータ(護衛対象の座標など) **///

	// 護衛対象の座標
	LWP::Object::WorldTransform* worldTransformTarget_ = nullptr;
	// ふっとばされるときのベクトル
	LWP::Math::Vector3 flyingVelocity_;

	/// ** フラグ関連 ** ///

	// 敵の生存フラグ( true == 生存 )
	bool isActive_;
	// 敵の浮遊フラグ( true == 浮遊してる )
	bool isFloat_;
	// 攻撃が命中したか( true == 命中した )
	bool isAttackHit_;
	// 外周に触れて反射したか( true == 反射した ,反射後、外周に触れるとfalseに再設定する )
	bool isReflection_;
	// 行動パターン( true == 反射時に護衛対象を追跡 , false == 常に護衛対象を追跡 )
	bool isActionTypeA_;

};