#include "Title.h"
#include "GameScene.h"

using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;

// 初期化
void Title::Initialize() {
	// テクスチャ読み込み
	uvTexture = LWP::Resource::LoadTexture("uvChecker.png");

	// title
	surface = LWP::Primitive::CreateInstance<Surface>();
	surface->isActive = true;
	surface->isUI = true;
	surface->texture = uvTexture;

	surface->vertices[0].position = {0.0f,0.0f,0.0f};
	surface->vertices[1].position = {1000.0f,0.0f,0.0f};
	surface->vertices[2].position = {1000.0f,250.0f,0.0f};
	surface->vertices[3].position = {0.0f,250.0f,0.0f };

	surface->transform.translation = { 140.0f ,20.0f ,0.0f };

	// ボタン
	for (int i = 0; i < 2; i++) {
		buttonModel[i] = LWP::Primitive::CreateInstance<Mesh>();
		buttonModel[i] = LWP::Resource::LoadModel("cube/cube.obj");
		buttonModel[i]->transform.scale = {1.0f,0.5f,0.2f};
		buttonModel[i]->transform.rotation = { 0.7f,0.0f,0.0f };
		buttonModel[i]->isActive = true;
	}
	buttonModel[0]->transform.translation = { -1.3f,-0.8f,2.0f };
	buttonModel[1]->transform.translation = { 1.2f,-0.8f,2.0f };

	//ハンマー
	hammerModel = LWP::Primitive::CreateInstance<Mesh>();
	hammerModel = LWP::Resource::LoadModel("hammer/hammer.obj");
	hammerModel->transform.translation = { -1.1f,-0.3f,0.65f };
	hammerModel->transform.rotation = { 0.6f,-0.1f,0.0f };
	hammerModel->transform.scale = { 0.3f, 0.3f, 0.3f };

	hammerModel->isActive = true;

}

// 更新
void Title::Update() {
	UpdateTimer();
	Shake();
	SceneChange();

	ImGui::Begin("Primitive");
	surface->DebugGUI("Surface");
	buttonModel[0]->DebugGUI("button");
	buttonModel[1]->DebugGUI("button");
	hammerModel->DebugGUI("hammerModel");
	ImGui::End();

	// カメラ操作
	ImGui::Begin("Camera");
	mainCamera->DebugGUI();
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
		hammerModel->transform.translation = { -1.1f,-0.3f,0.65f };
		selectPoint = true;
	}
	else if (LWP::Input::GetTrigger(DIK_D)) {
		hammerModel->transform.translation = { 1.2f,-0.3f,0.65f };
		selectPoint = false;
	}

	//次のシーンへ移動
	if (selectPoint && LWP::Input::GetTrigger(DIK_SPACE)) {
		sceneChangeTiemFlag = true;
		ButtonPush();
		shakeMaxPosition = 20;
	}

	//windowを閉じる
	if (selectPoint == 0 && LWP::Input::GetTrigger(DIK_SPACE)) {
		sceneChangeTiemFlag = true;
		ButtonPush();
		shakeMaxPosition = 20;
	}

}
// 描画
void Title::Draw() {
} 

void Title::UpdateTimer() {
	attackCoolTimer >= 0 ? attackCoolTimer-- : 0;
	attackAni.Progress(&hammerModel->transform);

	if (selectPoint) {
		buttonAni.Progress(&buttonModel[0]->transform);
	}
	else if (selectPoint == 0) {
		buttonAni.Progress(&buttonModel[1]->transform);

	}
}

void Title::SceneChange() {

	if (sceneChangeTiemFlag) {

		sceneChangeTime--;

		if (sceneChangeTime < 0) {
			nextScene_ = new GameScene();

			sceneChangeTime = 13;

			sceneChangeTiemFlag = false;

			return;
		}
	}

}

void Title::Shake() {
	shakeMaxPosition--;

	if (shakeMaxPosition < 1) {
		shakeMaxPosition = 1;
	}

	shakePosition = rand() % shakeMaxPosition - shakeMaxPosition / 2;

	surface->transform.translation = { 140.0f + shakePosition ,20.0f + shakePosition,0.0f + shakePosition };


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

void Title::ButtonPush() {
	// アニメーション開始
	buttonAni.timer = buttonAni.GetFullTIme();

}

void Title::Buttonanimation::Progress(LWP::Object::WorldTransform* transform) {
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

void Title::Easing() {
	
};