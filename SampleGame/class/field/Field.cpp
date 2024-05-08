#include "Field.h"

using namespace LWP;
using namespace LWP::Math;
using namespace LWP::Input;
using namespace LWP::Primitive;
using namespace LWP::Object;
using namespace LWP::Utility;

// 初期化
void Field::Init() {
	// スカイドーム
	skydome_.LoadFile("skydome/skydome.obj");
	skydome_.transform.scale = { 100.0f,100.0f, 100.0f };
	skydome_.name = "skydome";
	// 地面
	ground_.LoadFile("ground/Ground.gltf");
	ground_.transform.scale = { 100.0f,1.0f, 100.0f };
	ground_.material.enableLighting = true;
	ground_.name = "ground";

	// データを編集しやすいよう一括で並べとく
	WorldTransform data[kBuildingCount_] = {
		WorldTransform({-5.0f,0.0f,6.0f}, {0.0f,-1.0f,0.0f,0.0f}),
		WorldTransform({-2.8f,0.0f,6.0f}, {0.0f,-1.0f,0.0f,0.0f}),
		WorldTransform({-0.6f,0.0f,6.0f}, {0.0f,-1.0f,0.0f,0.0f}),
		WorldTransform({1.6f,0.0f,6.0f}, {0.0f,-1.0f,0.0f,0.0f}),
		WorldTransform({8.0f,0.0f,7.2f}, {0.0f,-0.923f,0.0f,0.386f}),
		WorldTransform({9.0f,0.0f,4.0f}, {0.0f,-0.731,0.0f,0.682f}),
		WorldTransform({9.0f,0.0f,1.8f}, {0.0f,-0.731,0.0f,0.682f}),
		WorldTransform({2.0f,0.0f,-10.0f}),
		WorldTransform({-0.2f,0.0f,-10.0f}),
		WorldTransform({-2.8f,0.0f,-10.0f})
	};

	// 建物初期化
	for (int i = 0; i < kBuildingCount_; i++) {
		building_[i].LoadFile("buildings/1Story.obj");
		building_[i].name = "Building" + std::to_string(i);
		building_[i].transform = data[i];
		building_[i].material.enableLighting = true;
	}

	// 木の種類
	std::string treePath[5] = {
		"nature/tree/BirchTree_1.obj",
		"nature/tree/BirchTree_2.obj",
		"nature/tree/BirchTree_3.obj",
		"nature/tree/BirchTree_4.obj",
		"nature/tree/BirchTree_5.obj"
	};

	// 木の初期化
	for (int i = 0; i < kTreeCount_; i++) {
		trees_[i].LoadFile(treePath[GenerateRandamNum<int>(0, 4)]);
		trees_[i].name = "Tree" + std::to_string(i);
		trees_[i].transform.translation.x = GenerateRandamNum<int>(-200, 200) / 10.0f;
		trees_[i].transform.translation.z = GenerateRandamNum<int>(-200, 200) / 10.0f;
		trees_[i].transform.rotation *= Quaternion::ConvertEuler({0.0f, GenerateRandamNum<int>(0, 614) / 100.0f, 0.0f});
		trees_[i].material.enableLighting = true;
	}
}
