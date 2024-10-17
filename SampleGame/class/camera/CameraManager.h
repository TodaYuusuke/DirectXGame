#pragma once
#include <Adapter.h>


/// <summary>
/// ゲームシーン中のカメラを制御するクラス
/// </summary>
class CameraManager final {
public:

	
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(LWP::Resource::LevelData* level, LWP::Object::Camera* camera);
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private: // ** メンバ変数 ** //
	// 保持するポインタ
	LWP::Resource::LevelData* level_;
	LWP::Object::Camera* camera_;

private: // ** メンバ関数 ** //
};