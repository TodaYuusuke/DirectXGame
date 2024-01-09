#include "HandGun.h"
#include <ImGuiManager.h>
#include "../../GlobalVariables/GlobalVariables.h"
#include "../../Utility/Math/Math.h"

void HandGun::Initialize(const WorldTransform& worldTransform) {
	bodyModel_.reset(Model::CreateFromOBJ("handgun_body", true));
	sliderModel_.reset(Model::CreateFromOBJ("handgun_slider", true));

	// 親子関係の登録
	bodyWT_.Initialize();
	bodyWT_.parent_ = &worldTransform;
	sliderWT_.Initialize();
	sliderWT_.parent_ = &bodyWT_;

	SetModelNeutral();

	// レティクルのワールドトランスフォーム初期化
	reticleWorldTransform.Initialize();
	reticleWorldTransform.translation_.z += 50.0f;
	reticleWorldTransform.parent_ = &worldTransform;
	reticleWorldTransform.UpdateMatrix();
	// スプライト生成
	reticleSprite.reset(Sprite::Create(
	    TextureManager::Load("reticle.png"), {1280.0f / 2.0f, 720.0f / 2.0f}, {1, 1, 1, 1},
	    {0.5f, 0.5f}));
	redDotSprite.reset(Sprite::Create(
	    TextureManager::Load("dotsight.png"), {1280.0f / 2.0f, 720.0f / 2.0f}, {1, 1, 1, 1},
	    {0.5f, 0.5f}));

	// サウンドデータの読み込み
	audio_ = Audio::GetInstance();
	soundDataHandle_[Fire] = audio_->LoadWave("audio/gamePlay/Fire.wav");
	soundDataHandle_[Reload1] = audio_->LoadWave("audio/gamePlay/Reload1.wav");
	soundDataHandle_[Reload2] = audio_->LoadWave("audio/gamePlay/Reload2.wav");
	soundDataHandle_[EmptyAmmo] = audio_->LoadWave("audio/gamePlay/EmptyAmmo.wav");

	// スプライト読み込み
	secondDigitNumberSprite_[0].reset(Sprite::Create(
	    TextureManager::Load("number/0.png"), {45.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	secondDigitNumberSprite_[1].reset(Sprite::Create(
	    TextureManager::Load("number/1.png"), {45.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[0].reset(Sprite::Create(
	    TextureManager::Load("number/0.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[1].reset(Sprite::Create(
	    TextureManager::Load("number/1.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[2].reset(Sprite::Create(
	    TextureManager::Load("number/2.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[3].reset(Sprite::Create(
	    TextureManager::Load("number/3.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[4].reset(Sprite::Create(
	    TextureManager::Load("number/4.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[5].reset(Sprite::Create(
	    TextureManager::Load("number/5.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[6].reset(Sprite::Create(
	    TextureManager::Load("number/6.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[7].reset(Sprite::Create(
	    TextureManager::Load("number/7.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[8].reset(Sprite::Create(
	    TextureManager::Load("number/8.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	firstDigitNumberSprite_[9].reset(Sprite::Create(
	    TextureManager::Load("number/9.png"), {110.0f, 625.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	slashSprite_.reset(Sprite::Create(
	    TextureManager::Load("number/slash.png"), {165.0f, 670.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	maxAmmoNumSprite_[0].reset(Sprite::Create(
	    TextureManager::Load("number/1.png"), {200.0f, 680.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	maxAmmoNumSprite_[0]->SetSize({36.0f, 44.0f});
	maxAmmoNumSprite_[1].reset(Sprite::Create(
	    TextureManager::Load("number/3.png"), {230.0f, 680.0f}, {1, 1, 1, 1}, {0.5f, 0.5f}));
	maxAmmoNumSprite_[1]->SetSize({36.0f, 44.0f});


	// json登録
	GlobalVariables* g = GlobalVariables::GetInstance();
	g->CreateGroup("HandGun");
	g->AddItem("HandGun", "VerticalRecoilPower", verticalRecoilPower_);
	g->AddItem("HandGun", "HorizontalRecoilPower", horizontalRecoilPower_);
	g->AddItem("HandGun", "RecoilFrame", kRecoilFrame_);
	g->AddItem("HandGun", "RecoilRotationX", kRecoilRotationX_);
	g->AddItem("HandGun", "SliderTranslationZ", kRecoilSliderTranslationZ_);
	g->AddItem("HandGun", "ADSFrame", kADSFrame_);
	g->AddItem("HandGun", "ADSTranslation", kADSTranslation_);
	g->AddItem("HandGun", "MAXAmmo", kMAXAmmo_);
	g->AddItem("HandGun", "ReloadIOFrame", kReloadIOFrame_);
	g->AddItem("HandGun", "ReloadWaitFrame", kReloadWaitFrame_);
	g->AddItem("HandGun", "ReloadTranslationY", kReloadTranslationY_);
	g->AddItem("HandGun", "ReloadRotationX", kReloadRotationX_);
}
void HandGun::Update() {
	RecoilAnimation();
	ADSAnimation();
	ReloadAnimation();

	// 調整項目適応
	ApplyGlobalVariables();

	bodyWT_.UpdateMatrix();
	sliderWT_.UpdateMatrix();
	reticleWorldTransform.UpdateMatrix();
}
void HandGun::Draw(const ViewProjection& viewProjection) {
	bodyModel_->Draw(bodyWT_, viewProjection);
	sliderModel_->Draw(sliderWT_, viewProjection);
}
void HandGun::DrawUI() {
	if (reloadFrame_ < 0) {
		if (adsFrame_ == kADSFrame_) {
			redDotSprite->Draw();
		} else {
			reticleSprite->Draw();
		}
	}

	// 装弾数を描画
	secondDigitNumberSprite_[ammo_ / 10]->Draw();
	firstDigitNumberSprite_[ammo_ % 10]->Draw();
	slashSprite_->Draw();
	maxAmmoNumSprite_[0]->Draw();
	maxAmmoNumSprite_[1]->Draw();
}

bool HandGun::Shot(Vector3* recoil) {
	// リコイルがまだ戻っていないなら or リロード中ならば何もしない
	if (recoilFrame_ >= 0 || reloadFrame_ >= 0) {
		return false;
	}
	// 装弾数がなければ何もしない
	if (ammo_ <= 0) {
		// 弾切れの音
		audio_->PlayWave(soundDataHandle_[EmptyAmmo], false, 0.3f);
		return false;
	}
	// 装弾数-1
	ammo_--;
	// リロード用のアニメーションフレーム初期化
	recoilFrame_ = 0;

	// リコイル量計測
	*recoil += verticalRecoilPower_;
	recoil->y += Math::RandomFloat(-horizontalRecoilPower_, horizontalRecoilPower_);
	// 銃声
	audio_->PlayWave(soundDataHandle_[Fire], false, 0.3f);

	return true;
}
void HandGun::SwitchADS() {
	// リロード中ならば何もしない
	if (reloadFrame_ >= 0) {
		return;
	}

	isADS = !isADS;
}

void HandGun::Reload() {
	// リロード中ならば何もしない
	if (reloadFrame_ >= 0 || ammo_ >= kMAXAmmo_) {
		return;
	}

	reloadFrame_ = 0;
	//isADS = false;
}


void HandGun::SetModelNeutral() {
	bodyWT_.translation_ = {0.75f, -0.5f, 1.2f};
	bodyWT_.rotation_ = {0.0f, 0.0f, 0.0f};
	bodyWT_.scale_ = {0.1f, 0.1f, 0.1f};
	sliderWT_.translation_ = {0.0f, 0.0f, 0.0f};
	sliderWT_.rotation_ = {0.0f, 0.0f, 0.0f};
	sliderWT_.scale_ = {1.0f, 1.0f, 1.0f};
}
void HandGun::ApplyGlobalVariables() {
	GlobalVariables* g = GlobalVariables::GetInstance();
	verticalRecoilPower_ = g->GetVector3Value("HandGun", "VerticalRecoilPower");
	horizontalRecoilPower_ = g->GetFloatValue("HandGun", "HorizontalRecoilPower");
	kRecoilFrame_ = g->GetIntValue("HandGun", "RecoilFrame");
	kRecoilRotationX_ = g->GetFloatValue("HandGun", "RecoilRotationX");
	kRecoilSliderTranslationZ_ = g->GetFloatValue("HandGun", "SliderTranslationZ");
	kADSFrame_ = g->GetIntValue("HandGun", "ADSFrame");
	kADSTranslation_ = g->GetVector3Value("HandGun", "ADSTranslation");
	kMAXAmmo_ = g->GetIntValue("HandGun", "MAXAmmo");
	kReloadIOFrame_ = g->GetIntValue("HandGun", "ReloadIOFrame");
	kReloadWaitFrame_ = g->GetIntValue("HandGun", "ReloadWaitFrame");
	kReloadTranslationY_ = g->GetFloatValue("HandGun", "ReloadTranslationY");
	kReloadRotationX_ = g->GetFloatValue("HandGun", "ReloadRotationX");
}

void HandGun::RecoilAnimation() {
	if (recoilFrame_ < 0) {
		return;
	}

	if (recoilFrame_ == 0) {
		if (!isADS)
			bodyWT_.rotation_.x += kRecoilRotationX_;
		sliderWT_.translation_.z += kRecoilSliderTranslationZ_;
	} else if (recoilFrame_ <= kRecoilFrame_) {
		if (!isADS)
			bodyWT_.rotation_.x -= kRecoilRotationX_ / kRecoilFrame_;
		if (ammo_ > 0) 
			sliderWT_.translation_.z -= kRecoilSliderTranslationZ_ / kRecoilFrame_;
	} else {
		bodyWT_.rotation_.x = 0.0f;
		if (ammo_ > 0)
			sliderWT_.translation_.z = 0.0f;
		recoilFrame_ = -1;
		return;
	}

	recoilFrame_++;
}
void HandGun::ADSAnimation() {
	Vector3 adsMove = { kADSTranslation_.x / kADSFrame_, kADSTranslation_.y / kADSFrame_, kADSTranslation_.z / kADSFrame_ };
	
	if (isADS) {
		if (adsFrame_ < kADSFrame_) {
			bodyWT_.translation_ += adsMove;
			adsFrame_++;
		}
	} else {
		if (adsFrame_ > 0) {
			bodyWT_.translation_ -= adsMove;
			adsFrame_--;
		}
	}
}
void HandGun::ReloadAnimation() {
	if (reloadFrame_ < 0) {
		return;
	}

	// SE関連
	if (reloadFrame_ == 0) {
		// リロード音1
		audio_->PlayWave(soundDataHandle_[Reload1], false, 0.3f);
	} else if (reloadFrame_ == kReloadIOFrame_ + kReloadWaitFrame_) {
		// リロード音2
		audio_->PlayWave(soundDataHandle_[Reload2], false, 0.3f);
		if (ammo_ <= 0)
			sliderWT_.translation_.z = 0.0f;
	}


	// アニメーション
	if (reloadFrame_ < kReloadIOFrame_) {
		bodyWT_.translation_.y += kReloadTranslationY_ / kReloadIOFrame_;
		bodyWT_.rotation_.x += kReloadRotationX_ / kReloadIOFrame_;
	}
	else if (reloadFrame_ < kReloadIOFrame_ + kReloadWaitFrame_) {
	
	}
	else if (reloadFrame_ < kReloadIOFrame_ + kReloadWaitFrame_ + kReloadIOFrame_) {
		bodyWT_.translation_.y -= kReloadTranslationY_ / kReloadIOFrame_;
		bodyWT_.rotation_.x -= kReloadRotationX_ / kReloadIOFrame_;
	}
	else {
		bodyWT_.rotation_.x = 0.0f;
		reloadFrame_ = -1;
		ammo_ = kMAXAmmo_;
		return;
	}

	reloadFrame_++;
}
