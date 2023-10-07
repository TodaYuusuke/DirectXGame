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
	// モデル(テストターゲット)
	testTargetModel_ = LWP::Engine::CreateModelInstance("testEnemy.obj");
	testTargetModel_->transform.rotation.x = 0.5f;
	// モデル(テストフィールド)
	testFieldModel_ = LWP::Engine::CreateModelInstance("testField.obj");
	testFieldModel_->transform.rotation.x = -1.571f;
	testFieldModel_->transform.scale = { fieldRadius,fieldRadius,fieldRadius };
	testFieldModel_->transform.translation = fieldCenterPos;

	// 敵の座標
	worldTransform_ = new LWP::Object::WorldTransform;
	worldTransform_->translation = { 8.0f,8.0f,50.0f };
	worldTransform_->rotation = { 1.0f,0.0f,0.0f };
	// 敵の半径
	radius_ = { 5.0f,5.0f,5.0f };
	// 移動量
	velocity_ = {};
	// 護衛対象の座標(ポインタで取得)
	worldTransformTarget_ = targetPos;
	worldTransformTarget_->translation = { 0.0f,0.0f,50.0f };

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

	ImGui::Begin("Target");
	worldTransformTarget_->DebugGUI();
	ImGui::End();

	ImGui::Begin("TestField");
	testFieldModel_->DebugGUI();
	ImGui::End();

	// アクティブ時の処理
	if (isActive_) {

		// 非浮遊時の処理
		if (!isFloat_) {

			// 移動処理
			// 座標系のyとzが逆になる可能性あり(現在はz軸が高さになっている)
			// 現時点だとサイドビューのため、そのまま値を代入しているので必要に応じ変更すること

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
				// Speedなどの変数が必要になったら追加すること
				velocity_ = subtractPos * 0.1f;

				// 反射フラグをtrueに
				isReflection_ = true;

			}

			// 移動する
			worldTransform_->translation.x += velocity_.x;
			worldTransform_->translation.y += velocity_.y;


			// 外周に触れていたら反射フラグをfalseに
			// 右側 : ( 外周の中心 + フィールド半径 - 敵の半径 ) <= 敵の座標 であれば外周に触れるはずなので反転させる
			// 多分ミスってるので修正予定
			if ((fieldCenterPos.x - fieldRadius + radius_.x) >= worldTransform_->translation.x ||
				(fieldCenterPos.y - fieldRadius + radius_.y) >= worldTransform_->translation.y ||
				(fieldCenterPos.x + fieldRadius - radius_.x) <= worldTransform_->translation.x ||
				(fieldCenterPos.y + fieldRadius - radius_.y) <= worldTransform_->translation.y) {
				
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
			if (worldTransform_->translation.z >= 0.0f) {

				// 座標を修正
				worldTransform_->translation.z = 60.0f;

				// 浮遊フラグをfalseに
				isFloat_ = false;
			}

		}

	}

	// 敵の座標系などをモデルに反映させる
	model_->transform.scale = worldTransform_->scale;				// 拡大縮小
	model_->transform.rotation = worldTransform_->rotation;			// 回転
	model_->transform.translation = worldTransform_->translation;	// 平行移動

	testTargetModel_->transform.translation = worldTransformTarget_->translation;	// 平行移動

}

/// 描画
void Enemy::Draw(){

	testFieldModel_->Draw();
	testTargetModel_->Draw();
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
