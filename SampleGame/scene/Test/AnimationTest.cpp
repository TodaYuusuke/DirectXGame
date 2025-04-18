#include "AnimationTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void AnimationTest::Initialize() {
	for (int i = 0; i < 5; i++) {
		model[i].LoadShortPath("Player/Robot/Player_Boned_IK.gltf");
		model[i].worldTF.translation.x = i * 1.5f;
		motion[i].Add(
			&model[i].worldTF.translation,
			{ -2.526f, 0.0f, 0.0f },
			0.0f, 2.0f
		);
		motion[i].Add(
			&model[i].worldTF.translation,
			{ 2.526f, 0.0f, 0.0f },
			2.0f, 2.0f
		);
		motion[i].Start(true);
	}
	anim.LoadFullPath("resources/model/Player/Robot/Player_Boned_IK.gltf", &model[0]);

	texture = Resource::LoadTexture("TestSpriteSheet.png");
}

// 更新
void AnimationTest::Update() {

}