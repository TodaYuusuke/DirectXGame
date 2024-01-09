#include "HandGun.h"
#include "../../GlobalVariables/GlobalVariables.h"

using namespace LWP::Math;
using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Object;

void HandGun::Initialize(WorldTransform* worldTransform) {
	bodyModel_ = LoadModel("handgun_body/handgun_body.obj");
	sliderModel_ = LoadModel("handgun_slider/handgun_slider.obj");

	// 親子関係の登録
	bodyModel_->transform.Parent(worldTransform);
	bodyModel_->material.enableLighting = true;
	sliderModel_->transform.Parent(&bodyModel_->transform);
	sliderModel_->material.enableLighting = true;

	SetModelNeutral();

	// レティクルの生成
	reticleSprite = CreateInstance<Sprite>();
	reticleSprite->transform.translation.x = LWP::Info::GetWindowWidthF() / 2.0f;
	reticleSprite->transform.translation.y = LWP::Info::GetWindowHeightF() / 2.0f;
	reticleSprite->anchorPoint = { 0.5f,0.5f };
	reticleSprite->texture = LoadTexture("reticle.png");
	reticleSprite->isUI = true;
	reticlePosition.translation.z = 50.0f;
	reticlePosition.Parent(worldTransform);
	// レッドドット生成
	redDotSprite = CreateInstance<Sprite>();
	redDotSprite->transform.translation.x = LWP::Info::GetWindowWidthF() / 2.0f;
	redDotSprite->transform.translation.y = LWP::Info::GetWindowHeightF() / 2.0f;
	redDotSprite->anchorPoint = { 0.5f,0.5f };
	redDotSprite->texture = LoadTexture("dotsight.png");
	redDotSprite->isUI = true;

	// サウンドデータの読み込み
	//soundDataHandle_[Fire] = audio_->LoadWave("audio/gamePlay/Fire.wav");
	//soundDataHandle_[Reload1] = audio_->LoadWave("audio/gamePlay/Reload1.wav");
	//soundDataHandle_[Reload2] = audio_->LoadWave("audio/gamePlay/Reload2.wav");
	//soundDataHandle_[EmptyAmmo] = audio_->LoadWave("audio/gamePlay/EmptyAmmo.wav");

	// スプライト読み込み
	for (int i = 0; i < 2; i++) {
		secondDigitNumberSprite_[i] = CreateInstance<Sprite>();
		secondDigitNumberSprite_[i]->texture = LoadTexture("number/" + std::to_string(i) + ".png");
		secondDigitNumberSprite_[i]->isUI = true;
		secondDigitNumberSprite_[i]->isActive = false;
	}
	for (int i = 0; i < 10; i++) {
		firstDigitNumberSprite_[i] = CreateInstance<Sprite>();
		firstDigitNumberSprite_[i]->texture = LoadTexture("number/" + std::to_string(i) + ".png");
		firstDigitNumberSprite_[i]->isUI = true;
		firstDigitNumberSprite_[i]->isActive = false;
	}

	slashSprite_ = CreateInstance<Sprite>();
	slashSprite_->texture = LoadTexture("number/slash.png");
	slashSprite_->isUI = true;
	slashSprite_->isActive = false;

	maxAmmoNumSprite_[0] = CreateInstance<Sprite>();
	maxAmmoNumSprite_[0]->texture = LoadTexture("number/1.png");
	maxAmmoNumSprite_[0]->isUI = true;
	maxAmmoNumSprite_[0]->isActive = false;
	maxAmmoNumSprite_[1] = CreateInstance<Sprite>();
	maxAmmoNumSprite_[1]->texture = LoadTexture("number/3.png");
	maxAmmoNumSprite_[1]->isUI = true;
	maxAmmoNumSprite_[1]->isActive = false;

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

	// レッドドットやレティクルの表示切り替え処理
	if (reloadFrame_ < 0) {
		if (adsFrame_ == kADSFrame_) {
			redDotSprite->isActive = true;
			reticleSprite->isActive = false;
		}
		else {
			reticleSprite->isActive = true;
			redDotSprite->isActive = false;
		}
	}
	// もしレッドドットを表示しているならば消す
	else {
		redDotSprite->isActive = false;
	}
}

bool HandGun::Shot(Vector3* recoil) {
	// リコイルがまだ戻っていないなら or リロード中ならば何もしない
	if (recoilFrame_ >= 0 || reloadFrame_ >= 0) {
		return false;
	}
	// 装弾数がなければ何もしない
	if (ammo_ <= 0) {
		// 弾切れの音
		//audio_->PlayWave(soundDataHandle_[EmptyAmmo], false, 0.3f);
		return false;
	}
	// 装弾数-1
	ammo_--;
	// リロード用のアニメーションフレーム初期化
	recoilFrame_ = 0;

	// リコイル量計測
	*recoil += verticalRecoilPower_;
	recoil->y += LWP::Utility::GenerateRandamNum<int>(int(-horizontalRecoilPower_ * 100), int(horizontalRecoilPower_ * 100)) / 100.0f;
	// 銃声
	//audio_->PlayWave(soundDataHandle_[Fire], false, 0.3f);

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
	bodyModel_->transform.translation = { 0.730f, -0.550f, 2.070f};
	bodyModel_->transform.rotation = {0.0f, 0.0f, 0.0f};
	bodyModel_->transform.scale = {0.1f, 0.1f, 0.1f};
	sliderModel_->transform.translation = {0.0f, 0.0f, 0.0f};
	sliderModel_->transform.rotation = {0.0f, 0.0f, 0.0f};
	sliderModel_->transform.scale = {1.0f, 1.0f, 1.0f};
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
			bodyModel_->transform.rotation.x += kRecoilRotationX_;
		sliderModel_->transform.translation.z += kRecoilSliderTranslationZ_;
	} else if (recoilFrame_ <= kRecoilFrame_) {
		if (!isADS)
			bodyModel_->transform.rotation.x -= kRecoilRotationX_ / kRecoilFrame_;
		if (ammo_ > 0) 
			sliderModel_->transform.translation.z -= kRecoilSliderTranslationZ_ / kRecoilFrame_;
	} else {
		bodyModel_->transform.rotation.x = 0.0f;
		if (ammo_ > 0)
			sliderModel_->transform.translation.z = 0.0f;
		recoilFrame_ = -1;
		return;
	}

	recoilFrame_++;
}
void HandGun::ADSAnimation() {
	Vector3 adsMove = { kADSTranslation_.x / kADSFrame_, kADSTranslation_.y / kADSFrame_, kADSTranslation_.z / kADSFrame_ };
	
	if (isADS) {
		if (adsFrame_ < kADSFrame_) {
			bodyModel_->transform.translation += adsMove;
			adsFrame_++;
		}
	} else {
		if (adsFrame_ > 0) {
			bodyModel_->transform.translation -= adsMove;
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
		//audio_->PlayWave(soundDataHandle_[Reload1], false, 0.3f);
	} else if (reloadFrame_ == kReloadIOFrame_ + kReloadWaitFrame_) {
		// リロード音2
		//audio_->PlayWave(soundDataHandle_[Reload2], false, 0.3f);
		if (ammo_ <= 0)
			sliderModel_->transform.translation.z = 0.0f;
	}


	// アニメーション
	if (reloadFrame_ < kReloadIOFrame_) {
		bodyModel_->transform.translation.y += kReloadTranslationY_ / kReloadIOFrame_;
		bodyModel_->transform.rotation.x += kReloadRotationX_ / kReloadIOFrame_;
	}
	else if (reloadFrame_ < kReloadIOFrame_ + kReloadWaitFrame_) {
	
	}
	else if (reloadFrame_ < kReloadIOFrame_ + kReloadWaitFrame_ + kReloadIOFrame_) {
		bodyModel_->transform.translation.y -= kReloadTranslationY_ / kReloadIOFrame_;
		bodyModel_->transform.rotation.x -= kReloadRotationX_ / kReloadIOFrame_;
	}
	else {
		bodyModel_->transform.rotation.x = 0.0f;
		reloadFrame_ = -1;
		ammo_ = kMAXAmmo_;
		return;
	}

	reloadFrame_++;
}
