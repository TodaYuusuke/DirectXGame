#pragma once
#include <Adapter.h>
#include "class/Behavior.h"

/// <summary>
/// ゲームシーン中のカメラを制御するクラス
/// </summary>
class CameraManager final {
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(
		LWP::Resource::LevelData* level,
		LWP::Object::Camera* camera,
		LWP::Utility::StatePattern<BehaviorGS, size_t(BehaviorGS::Count)>* state);
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private: // ** メンバ変数 ** //
	// 保持するポインタ
	LWP::Resource::LevelData* level_;
	LWP::Object::Camera* camera_;
	LWP::Utility::StatePattern<BehaviorGS, size_t(BehaviorGS::Count)>* state_;

	// 使用するアニメーション用の曲線のポインタ
	LWP::Utility::CatmullRom* catmullRom_ = nullptr;

private: // ** メンバ関数 ** //
};