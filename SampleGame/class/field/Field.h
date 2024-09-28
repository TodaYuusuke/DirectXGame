#pragma once
#include <Adapter.h>


/// <summary>
/// 地形をまとめたクラス
/// </summary>
class Field final {
public:

	// 初期化
	void Init(LWP::Resource::LevelData* level, LWP::Object::Camera* camera);

private: // ** メンバ変数 ** //

	// スカイドーム
	LWP::Resource::RigidModel skydome;
	// ☆
	const static int kStarCount = 512;
	LWP::Resource::RigidModel stars[kStarCount];

	// 太陽
	LWP::Object::DirectionLight sun;
};