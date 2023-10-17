#include "Title.h"
#include "GameScene.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTexture("uvChecker.png");

	// 平面
	for (int i = 0; i < 3; i++) {
		surface[i] = LWP::Primitive::CreateInstance<Surface>();
		surface[i]->isActive = true;
		surface[i]->isUI = true;
		surface[i]->texture = uvTexture;
	}

	surface[0]->vertices[0].position = {0.0f,0.0f};
	surface[0]->vertices[1].position = {1000.0f,0.0f};
	surface[0]->vertices[2].position = {1000.0f,250.0f};
	surface[0]->vertices[3].position = {0.0f,250.0f};

	surface[1]->vertices[0].position = { 0.0f,0.0f };
	surface[1]->vertices[1].position = { 300.0f,0.0f };
	surface[1]->vertices[2].position = { 300.0f,100.0f };
	surface[1]->vertices[3].position = { 0.0f,100.0f };
	
	surface[2]->vertices[0].position = { 0.0f,0.0f };
	surface[2]->vertices[1].position = { 300.0f,0.0f };
	surface[2]->vertices[2].position = { 300.0f,100.0f };
	surface[2]->vertices[3].position = { 0.0f,100.0f };

	surface[0]->transform.translation = { 140.0f,20.0f };
	surface[1]->transform.translation = { 490.0f,350.0f };
	surface[2]->transform.translation = { 490.0f,550.0f };

	//ハンマー
	hammerModel->transform.translation.y = 1.0f;
	hammerModel->transform.scale = { 0.1f, 0.1f, 0.1f };
	// 半透明に
	hammerModel->commonColor = new Color(0xFFFFFF64);

	// モデル読み込み
	hammerModel = LWP::Resource::LoadModel("hammer/hammer.obj");
	hammerModel->isActive = true;

}
// 更新
void Title::Update() {
	UpdateTimer();

	ImGui::Begin("Primitive");
	surface[0]->DebugGUI("Surface");
	surface[1]->DebugGUI("Surface");
	surface[2]->DebugGUI("Surface");
	ImGui::End();


	// ENTERキーを押すとシーン切り替え
	if (LWP::Input::GetTrigger(DIK_RETURN)) {
		nextScene_ = new GameScene();
	}

	// SPACEで攻撃
	if (LWP::Input::GetTrigger(DIK_SPACE) && attackCoolTimer < 0) {
		Attack();
	}

	if (LWP::Input::GetTrigger(DIK_A)) {

	}

	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
	ImGui::End();
}
// 描画
void Title::Draw() {
} 

void Title::UpdateTimer() {
	attackCoolTimer >= 0 ? attackCoolTimer-- : 0;
	attackAni.Progress(&hammerModel->transform);
}

void Title::Attack() {
	// アニメーション開始
	attackAni.timer = attackAni.GetFullTIme();

	// 攻撃クールタイム
	attackCoolTimer = 30;
}

void Title::AttackAnimation::Progress(LWP::Object::WorldTransform* transform) {
	timer--;
	if (timer < 0) { return; }

	// 振り下ろしアニメーション
	if (timer >= swingUp.time) {
		*transform += swingDown.transform / static_cast<float>(swingDown.time);
	}
	else {
		*transform += swingUp.transform / static_cast<float>(swingUp.time);
	}
}