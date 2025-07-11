#pragma once
#include <Adapter.h>
#include "scene/IScene.h"

#include "../../class/field/Field.h"

class ColliderTest final
	: public IScene {
public:	// ** メンバ関数 ** //

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ColliderTest();


public:	// ** 純粋仮想関数の実体宣言 ** //

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;


private: //*** これより先に必要な処理や変数を記述 ***//

	// 地形データ
	//Field field_;

	//LWP::Object::DirectionLight sun_;
	LWP::Object::GPUParticle blood_;
	LWP::Object::GPUParticle deadBody_;
	LWP::Object::GPUParticle bloodFall_;
	LWP::Resource::StaticModel mesh_;
	LWP::Resource::StaticModel bill_;
	LWP::Resource::StaticModel cube_[2];

	LWP::Math::Vector3 offset;

	bool addBlood = false;
	bool addDeadBody = false;
	bool addBloodFall = true;

	LWP::Object::DirectionLight light;

	// パーティクルの継続生成
	int time = 0;
};