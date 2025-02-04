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
	Field field_;

	LWP::Object::GPUParticle particle_;

	LWP::Resource::StaticModel mesh;
	LWP::Object::Collision meshCol;
	LWP::Object::Collision aabbCol;
	LWP::Object::Collision pointCol;
};