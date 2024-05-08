#pragma once
#include <Adapter.h>

class Field final {
public:

	// 初期化
	void Init();


private:

	// スカイドーム
	LWP::Primitive::Mesh skydome_;
	// 地面
	LWP::Primitive::Mesh ground_;
	// 建物
	static const int kBuildingCount_ = 10;	// 数
	LWP::Primitive::Mesh building_[kBuildingCount_];	// モデル

	// 木
	static const int kTreeCount_ = 10;	// 数
	LWP::Primitive::Mesh trees_[kTreeCount_];	// モデル
};