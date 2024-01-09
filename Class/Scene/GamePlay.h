#pragma once
#include "scene/IScene.h"
//#include "DebugCamera.h"
//#include "MyClass/GameObject/FPSCamera/FPSCamera.h"

#include "../GameObject/Character/CharacterManager.h"

class GamePlay final : public IScene {
public:
	//*** 純粋仮想関数の実体宣言 ***//

	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;

	// scene遷移したい場合、ISceneポインタ型のnextSceneに次のシーンの実体を代入

private: //*** これより先に必要な処理や変数を記述 ***//
	
	// スカイドーム
	LWP::Primitive::Mesh* skydome_ = nullptr;
	// グラウンド
	LWP::Primitive::Mesh* ground_ = nullptr;
	// 平行光源
	LWP::Object::DirectionLight* light_ = nullptr;


	// キャラクター管理クラス
	std::unique_ptr<CharacterManager> characterManager_ = nullptr;
};