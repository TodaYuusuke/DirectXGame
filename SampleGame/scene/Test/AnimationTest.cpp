#include "AnimationTest.h"

using namespace LWP;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Math;
using namespace LWP::Utility;
using namespace LWP::Object;

// 初期化
void AnimationTest::Initialize() {
	model.LoadShortPath("Player/Robot/Player_Boned_IK.gltf");
	anim.LoadFullPath("resources/model/Player/Robot/Player_Boned_IK.gltf", &model);


	motion.Add(
		&model.worldTF.translation,
		{ -2.526f, 0.0f, 0.0f },
		0.0f, 2.0f
	);
	motion.Add(
		&model.worldTF.translation,
		{ 2.526f, 0.0f, 0.0f },
		2.0f, 2.0f
	);
	motion.Start(true);
}

// 更新
void AnimationTest::Update() {

}