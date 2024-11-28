#pragma once
#include "BlackBelt.h"
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
		LWP::Utility::StatePattern<BehaviorGS, size_t(BehaviorGS::Count)>* state,
		int* wave
	);
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();


private: // ** メンバ変数 ** //

	// 保持するポインタ
	LWP::Resource::LevelData* level_ = nullptr;
	LWP::Object::Camera* camera_ = nullptr;
	LWP::Utility::StatePattern<BehaviorGS, size_t(BehaviorGS::Count)>* state_ = nullptr;
	int* wave_ = nullptr;

	// カメラの移動ルートのポインタ
	LWP::Utility::CatmullRom* route_ = nullptr;
	// 移動中に注視するルートのポインタ
	LWP::Utility::CatmullRom* focus_ = nullptr;

	// 上下の黒帯
	BlackBelt blackBelt_;


private: // ** プライベートなメンバ関数 ** //

	/// <summary>
	/// ウェーブを使い、読み込むルートの曲線名を生成
	/// </summary>
	/// <returns></returns>
	std::string GetRouteName();
	/// <summary>
	/// ウェーブを使い、読み込むフォーカスの曲線名を生成
	/// </summary>
	/// <returns></returns>
	std::string GetFocusName();
};