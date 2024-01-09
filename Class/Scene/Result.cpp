#include "Result.h"
#include "Title.h"
#include "GamePlay.h"

using namespace LWP::Primitive;
using namespace LWP::Resource;
using namespace LWP::Input;

// 初期化
void Result::Initialize() {
	// スプライト生成
	backGround_ = CreateInstance<Sprite>();
	backGround_->texture = LoadTexture("resultScene.png");
	backGround_->isUI = true;
}
// 更新
void Result::Update() {
	// Xトリガーを押していたら >>> 次のシーンへ
	if (Pad::GetTrigger(0, XBOX_X) || Keyboard::GetTrigger(DIK_SPACE)) {
		//gameStartSE_->Play();
		nextScene_ = new Title();
	}
	else if (Pad::GetTrigger(0, XBOX_Y) || Keyboard::GetTrigger(DIK_R)) {
		//gameStartSE_->Play();
		nextScene_ = new GamePlay();
	}
}