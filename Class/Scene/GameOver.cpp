#include "GameOver.h"
#include "Title.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Input;
using namespace LWP::Info;

void GameOver::Initialize() {
	Sprite* s = CreateInstance<Sprite>();
	s->texture = LoadTexture("GameOver.png");
	s->transform.translation = { GetWindowWidthF() / 2.0f,GetWindowHeightF() / 3.0f, 0.0f };
	s->anchorPoint = { 0.5f,0.5f };
	s->isUI = true;

	sprite_ = CreateInstance<Sprite>();
	sprite_->texture = LoadTexture("GameOverText.png");
	sprite_->transform.translation = { GetWindowWidthF() / 2.0f,GetWindowHeightF() / 3.0f * 2.0f, 0.0f };
	sprite_->anchorPoint = { 0.5f,0.5f };
	sprite_->isUI = true;

	// フルスクリーンに
	LWP::Window::ChangeFullScreenMode();
}

void GameOver::Update() {
	// 次のシーンへ
	if (Keyboard::GetTrigger(DIK_SPACE) || Pad::GetTrigger(XBOX_A)) {
		nextScene_ = new Title();
	}
	// プログラム終了
	if (Keyboard::GetTrigger(DIK_ESCAPE)) {
		LWP::System::End();
	}
}