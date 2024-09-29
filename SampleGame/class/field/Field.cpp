#include "Field.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;

// 初期化
void Field::Init(LWP::Resource::LevelData* level, LWP::Object::Camera* camera) {
	// レベルデータ読み込み
	level->SetScaleMultiply(10.0f);	// 地形は10倍サイズに
	level->LoadShortPath("SampleGameScene.json");

	// bloomをON
	/*camera->pp.use = true;
	camera->pp.outLine.use = true;
	camera->pp.bloom.use = true;
	camera->pp.CreateShaderFile();*/

	const float s = 1000.0f;
	skydome.LoadShortPath("skydome/skydome.obj");
	skydome.worldTF.scale = { s,s,s };
	//skydome.materials[1].color = Color(5, 5, 16, 255);
	skydome.SetAllMaterialLighting(false);
	for (int i = 0; i < kStarCount; i++) {
		stars[i].LoadCube();
		stars[i].worldTF.translation = Vector3{
			LWP::Utility::GenerateRandamNum<int>(-100, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(-100, 100) / 100.0f,
		}.Normalize() * (s - 1.0f);
		stars[i].worldTF.rotation = Quaternion{
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			LWP::Utility::GenerateRandamNum<int>(0, 100) / 100.0f,
			1.0f
		}.Normalize();
		float scale = LWP::Utility::GenerateRandamNum<int>(50, 100) / 100.0f;
		stars[i].worldTF.scale = { scale,scale,scale };

		//stars[i].materials[0].color = Utility::ColorPattern::YELLOW;
		stars[i].SetAllMaterialLighting(false);
	}

	sun.rotation.x = 0.0f;
}
