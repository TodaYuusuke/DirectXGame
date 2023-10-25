#include "Tambourine.h"

Tambourine::Tambourine() {

}

Tambourine::~Tambourine() {

}

void Tambourine::Initialize() {

	// タンバリン本体
	tamModel_ = LWP::Resource::LoadModel("tam/tam.obj");
	tamModel_->isActive = true;
	tamModel_->transform.scale = { 2.2f,2.4f,2.2f };
	tamModel_->transform.translation.y = -0.02f;

	// ベル
	bellModel_ = LWP::Resource::LoadModel("tam/bell.obj");
	bellModel_->isActive = true;
	bellModel_->transform.scale = { 2.2f,2.4f,2.2f };
	bellModel_->transform.translation.y = -0.02f;

	// ベルのシェイク量
	shakeBallRotate_ = { 0.0f,0.0f ,0.0f };
	shakeBallRotateMax_ = 0;
	shakeBallRotateNum_ = 0;

}
void Tambourine::Update() {

	ImGui::Begin("tambourine");
	tamModel_->DebugGUI();
	bellModel_->DebugGUI();
	ImGui::End();

	// 乱数を再設定
	srand((unsigned int)time(nullptr));

	// ベルを揺らす
	shakeBallRotateMax_--;
	if (shakeBallRotateMax_ < 1) {
		shakeBallRotateMax_ = 1;
	}

	shakeBallRotateNum_ = std::rand() % shakeBallRotateMax_ - shakeBallRotateMax_ / 2;
	shakeBallRotate_.x = (float)shakeBallRotateNum_ / 100.0f;

	// ベルの回転後
	bellModel_->transform.rotation.x = shakeBallRotate_.x;

}

void Tambourine::ShakeBell() {
	shakeBallRotateMax_ = 20;
}
