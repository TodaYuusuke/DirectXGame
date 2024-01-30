#include "Player.h"
#include <cassert>

using namespace LWP::Info;
using namespace LWP::Math;
using namespace LWP::Input;

// コンストラクタ
Player::Player() {

}
// デストラクタ
Player::~Player() { 
	// 弾をDELETE
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
}


void Player::Initialize(LWP::Object::Camera* cameraPtr) {
	// カメラのポインタをセット
	parentCamera_ = cameraPtr;

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);

	// モデル
	model_ = LWP::Resource::LoadModel("cube/cube.obj");
	model_->texture = LWP::Resource::LoadTexture("player.png");

	// ワールドトランスフォ－ムの初期化
	model_->transform.translation.z = 30.0f;
	// ペアレントをセット
	model_->transform.Parent(&cameraPtr->transform);
	
	// 3Dレティクルのワールドトランスフォーム初期化
	
	//worldTransform3DReticle_.Initialize();
	//worldTransform3DReticle_.translation_.z += 50.0f;
	//worldTransform3DReticle_.parent_ = &worldTransform_;
	//worldTransform3DReticle_.UpdateMatrix();

	// スプライト生成
	reticle_ = LWP::Primitive::CreateInstance<LWP::Primitive::Sprite>();
	reticle_->transform.translation = { GetWindowWidthF() / 2.0f, GetWindowHeightF() / 2.0f, 0.0f };
	reticle_->anchorPoint = { 0.5f,0.5f };
	reticle_->texture = LWP::Resource::LoadTexture("Reticle.png");
	reticle_->isActive = true;
	reticle_->isUI = true;
}



void Player::Update() {
	// 弾更新
	for (PlayerBullet* bullet : bullets_){
		bullet->Update();
	}
	// 射撃遅延カウントを減らす
	shotDelay_--;

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (!bullet->isAlive()) {
			delete bullet;
			return true;
		}
		return false;
	});

// ーーーーーーーーーーーーーーーーーー//
#pragma region Translation処理

	// 移動処理（キーボード）
	MoveKeyboard();
	// 移動処理（コントローラー）
	MoveController();

	//*--　　移動限界　　--*//

	// 移動限界座標
	const float kMoveLimitX = 20 / 2;
	const float kMoveLimitY = 10 / 2;
	
	// 範囲を超えない処理
	model_->transform.translation.x =
	    std::clamp(model_->transform.translation.x, -kMoveLimitX, +kMoveLimitX);
	model_->transform.translation.y =
	    std::clamp(model_->transform.translation.y, -kMoveLimitY, +kMoveLimitY);

	// レティクルのサイズの半分
	const float kReticleMoveLimitX = reticle_->size.t.x / 2.0f;
	const float kReticleMoveLimitY = reticle_->size.t.y / 2.0f;

	reticle_->transform.translation.x =
	    std::clamp(reticle_->transform.translation.x, 0.0f + kReticleMoveLimitX, GetWindowWidthF() - kReticleMoveLimitX);
	reticle_->transform.translation.y =
		std::clamp(reticle_->transform.translation.y, 0.0f + kReticleMoveLimitY, GetWindowHeightF() - kReticleMoveLimitY);

#pragma endregion
// ーーーーーーーーーーーーーーーーーー//
#pragma region rotation処理
	// 押した方向で移動ベクトルを変更
	if (Keyboard::GetPress(DIK_Q)) {
		model_->transform.rotation.y -= kRotSpeed;
	} else if (Keyboard::GetPress(DIK_E)) {
		model_->transform.rotation.y += kRotSpeed;
	}
#pragma endregion
// ーーーーーーーーーーーーーーーーーー//
#pragma region scale処理
#pragma endregion
// ーーーーーーーーーーーーーーーーーー//

}


void Player::OnCollision() {
	// 何もしない
}

Vector3 Player::GetWorldPosition() { return model_->transform.GetWorldPosition(); }


/*ーーーーーーーーーーーーーー*/
/*　　　　　　関数　　　　　　*/
/*ーーーーーーーーーーーーーー*/

void Player::MoveKeyboard() {
	// キャラクターの移動ベクトル
	Vector3 move{};
	// レティクルの座標
	Vector3 reticleMove{};

	// 押した方向で移動ベクトルを変更（左右）
	if (Keyboard::GetPress(DIK_A)) {
		move.x -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_D)) {
		move.x += 1.0f;
	}
	// 押した方向で移動ベクトルを変更（上下）
	if (Keyboard::GetPress(DIK_W)) {
		move.y += 1.0f;
	}
	if (Keyboard::GetPress(DIK_S)) {
		move.y -= 1.0f;
	}

	// レティクルも同様
	if (Keyboard::GetPress(DIK_LEFT)) {
		reticleMove.x -= 1.0f;
	}
	if (Keyboard::GetPress(DIK_RIGHT)) {
		reticleMove.x += 1.0f;
	}
	if (Keyboard::GetPress(DIK_DOWN)) {
		reticleMove.y += 1.0f;
	}
	if (Keyboard::GetPress(DIK_UP)) {
		reticleMove.y -= 1.0f;
	}


	// 移動
	model_->transform.translation += move * kCharacterSpeed;
	// レティクル移動
	reticle_->transform.translation += reticleMove * 15.0f;

	// マウス座標をスプライト座標にセット
	//sprite2DReticle_->SetPosition(GetMousePosition());

	// 射撃（SPACEトリガーを押していたら
	if (Keyboard::GetPress(DIK_SPACE)) {
		Attack();
	}
}

void Player::MoveController() {
	// キャラクターの移動ベクトル
	Vector2 move = Pad::GetLStick() * kCharacterSpeed;
	// レティクルの座標
	Vector2 reticleMove = Pad::GetRStick() * 15.0f;
	// Y軸反転
	reticleMove.y *= -1.0f;

	// 移動
	model_->transform.translation.x += move.x;
	model_->transform.translation.y += move.y;
	// スプライトの座標変更を反映
	reticle_->transform.translation.x += reticleMove.x;
	reticle_->transform.translation.y += reticleMove.y;

	// 射撃（Rトリガーを押していたら）
	if (Pad::GetPress(XBOX_RT)) {
		Attack();
	}
}

Vector3 Player::GetReticleTargetPosition() {
	// ビューポート行列
	Matrix4x4 matViewport = Matrix4x4::CreateViewportMatrix(0, 0, LWP::Info::GetWindowWidthF(), LWP::Info::GetWindowHeightF(), 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = parentCamera_->GetViewProjection() * matViewport;
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = matViewProjectionViewport.Inverse();

	// スプライト座標を受け取る
	Vector2 spritePosition = { reticle_->transform.GetWorldPosition().x,reticle_->transform.GetWorldPosition().y };

	// ニアクリップとファークリップの面上のワールド座標を得る
	Vector3 posNear = Vector3(spritePosition.x, spritePosition.y, 0);
	Vector3 posFar = Vector3(spritePosition.x, spritePosition.y, 1);

	// スクリーン→ワールド座標変換（ここで3Dから2Dになる）
	posNear = Matrix4x4::TransformCoord(posNear, matInverseVPV);
	posFar = Matrix4x4::TransformCoord(posFar, matInverseVPV);
	// マウスレイの方向
	Vector3 mouseDirection = Vector3{ posFar - posNear }.Normalize();
	// カメラから照準オブジェクトへの距離
	const float kDistanceTestObject = 100.0f;
	return posNear + mouseDirection * kDistanceTestObject;
}

void Player::Attack() {
	// 射撃ディレイ中じゃなければ発射
	if (shotDelay_ > 0) { return; }

	// 弾の速度
	Vector3 velocity{};

	velocity = GetReticleTargetPosition() - GetWorldPosition();
	velocity = velocity.Normalize() * kBulletSpeed;

	// 弾を生成し、初期化
	PlayerBullet* newBullet = new PlayerBullet();
	newBullet->Initialize(GetWorldPosition(), velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);

	shotDelay_ = kShotDelayTime;
}