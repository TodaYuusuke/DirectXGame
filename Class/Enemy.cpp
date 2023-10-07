#include "Enemy.h"


/// コンストラクタ
Enemy::Enemy(LWP::Object::WorldTransform* targetPos){
	this->Initialize(targetPos);
}

/// デストラクタ
Enemy::~Enemy(){}


/// 初期化
void Enemy::Initialize(LWP::Object::WorldTransform* targetPos){
	
	// モデル
	model_ = LWP::Engine::CreateModelInstance("testEnemy.obj");
	model_->transform.rotation.x = 0.5f;

	// 敵の座標
	worldTransform_ = new LWP::Object::WorldTransform;
	worldTransform_->translation = { 20.0f,12.0f,60.0f };
	// 敵の半径
	rad_ = { 5.0f,5.0f,5.0f };
	// 移動量
	velocity_ = {};
	// 護衛対象の座標(ポインタで取得)
	worldTransformTarget_ = targetPos;

	// 敵の体力
	hp_ = 100;

	// 敵の生存フラグ( true == 生存 )
	isActive_ = true;
	// 敵の浮遊フラグ( true == 浮遊してる )
	isFloat_ = false;
	//  攻撃が命中したか( true == 命中した )
	isAttackHit_ = false;
	// 外周に触れて反射したか( true == 反射した ,反射後、外周に触れるとfalseに再設定する )
	isReflection_ = false;
	// 行動パターン( true == 反射時に護衛対象を追跡 , false == 常に護衛対象を追跡 )
	isActionTypeA_ = true;

}

/// 更新
void Enemy::Update(){

	ImGui::Begin("Enemy");
	worldTransform_->DebugGUI();
	ImGui::End();

	// アクティブ時の処理
	if (isActive_) {

		// 非浮遊時の処理
		if (!isFloat_) {

			// 移動処理

			// 壁反射時に移動量を計算する
			if (!isReflection_) {

				// 差分を取る(移動量などはVector2で取る)
				LWP::Math::Vector2 subtractPos{};
				subtractPos.x = worldTransformTarget_->GetWorldPosition().x - worldTransform_->GetWorldPosition().x;
				subtractPos.y = worldTransformTarget_->GetWorldPosition().y - worldTransform_->GetWorldPosition().y;

				// 正規化する
				float len = sqrtf(subtractPos.x * subtractPos.x + subtractPos.y * subtractPos.y);
				subtractPos.x /= len;
				subtractPos.y /= len;

				// 1フレームあたりの速度に調整する
				velocity_ = subtractPos * 0.01f;

				// 反射フラグをtrueに
				isReflection_ = true;

			}

			// 移動する
			worldTransform_->translation.x += velocity_.x;
			worldTransform_->translation.y += velocity_.y;

			// 外周に触れていたら反射フラグをfalseに
			if (0) {
				isReflection_ = false;
			}

		}
		// 浮遊時の処理
		else if (isFloat_) {

			// 浮遊時間が残っている場合
			if (floatTime_ > 0) {

				// 浮遊する


			}

			// 重力の値だけ落ちる

			// 地面より下に落ちたら、座標を地面の上に戻した上でフラグを解除する
			if (worldTransform_->translation.z <= 0.0f) {

				// 座標を修正
				worldTransform_->translation.z = 0.0f;

				// 浮遊フラグをfalseに
				isFloat_ = false;
			}

		}

	}

	model_->transform.translation = worldTransform_->translation;

}

/// 描画
void Enemy::Draw(){

	model_->Draw();

}

/// 浮遊状態に変更する
void Enemy::SetFloat() { isFloat_ = true; }

/// 敵にダメージを与える
void Enemy::Damege(int32_t damege) { hp_ -= damege; }

/// 敵を即死させる
void Enemy::SetDeath() { isActive_ = false; }

/// 敵の生死を確認する
bool Enemy::GetIsActive() { return isActive_; }
