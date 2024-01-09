#pragma once
#include "../Effect/EffectManager.h"
#include "../Collision/Collider.h"

class Enemy : public Collider {
public: // メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(LWP::Math::Vector3 position);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	
	// エネミーの移動経路をセット
	void SetControlPoints(std::vector<LWP::Math::Vector3> controlPoints) { controlPoints_ = controlPoints; }
	// テクスチャハンドルをセット
	void SetTextureHandle(uint32_t enemyTextureHandle) { hitTextureHandle_ = enemyTextureHandle; }
	// プレイヤーのWorldTransformをセット
	void SetPlayerWorldTransform(LWP::Object::WorldTransform* playerWorldTransform) {
		playerWorldTransform_ = playerWorldTransform;
	}
	
	// isDeadを受け取る
	bool GetisDead() { return isDead_; }

	// 衝突時に呼ばれる関数
	void OnCollision() override;

	// キル数をカウントするためのポインタ
	int32_t* killCount_;

private: // メンバ定数
	float kCharacterSpeed = 0.3f;

private: // メンバ変数
	enum Parts {
		Core,
		Block1,
		Block2,
	};

	// 雑魚敵のモデル
	LWP::Primitive::Mesh* coreModel_;
	LWP::Primitive::Mesh* block1Model_;
	LWP::Primitive::Mesh* block2Model_;

	// テクスチャたち
	LWP::Resource::Texture* coreTex_;
	LWP::Resource::Texture* block1Tex_;
	LWP::Resource::Texture* block2Tex_;
	// 被弾時のテクスチャ
	LWP::Resource::Texture* hitTex_;


	// 移動するスプライン曲線制御点
	std::vector<LWP::Math::Vector3> controlPoints_;
	// 補間
	float t;

	// ヒットテクスチャハンドル
	uint32_t hitTextureHandle_;
	// 体力
	int32_t health = 3;
	// 生存フラグ
	bool isDead_ = false;

	// プレイヤーのWorldTransformのポインタ
	const LWP::Object::WorldTransform* playerWorldTransform_;

private: // プライベートな関数
	
	// モデルを初期値に戻す
	void SetModelNeutral();
	// 調整項目適応
	void ApplyGlobalVariables();

private: // アニメーション関連

	// 回転パラメータ
	LWP::Math::Vector3 kRotationSpeed_ = {0.01f, 0.01f, 0.01f};	// 現在の回転速度
	void Rotation();

	// 伸縮パラメータ
	float kElasticityMAXSize_ = 1.2f;
	float kElasticityMINSize_ = 1.0f;
	int32_t kElasticityCycleFrame_ = 60;
	float kElasticityT_ = 0.0f;
	bool kElasticityUpper = true;
	void Elasticity();

	// 被弾リアクションパラメータ
	int32_t kHitFrame_ = -1;
	void Hit();
};
