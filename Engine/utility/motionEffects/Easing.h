#pragma once
#include <functional>	// 関数ポインタ用
#include <string>		// 文字列用

namespace LWP::Utility::Easing {
	// 直線補間
	float Liner(float x);

#pragma region Quint
	float InQuint(float x);
	float OutQuint(float x);
	float InOutQuint(float x);
#pragma endregion

#pragma region Circ
	float InCirc(float x);
	float OutCirc(float x);
	float InOutCirc(float x);
#pragma endregion

#pragma region Cubic
	float InCubic(float x);
	float OutCubic(float x);
	float InOutCubic(float x);
#pragma endregion

#pragma region Back
	float InBack(float x);
	float OutBack(float x);
	float InOutBack(float x);
#pragma endregion

#pragma region Bounce
	float InBounce(float x);
	float OutBounce(float x);
	float InOutBounce(float x);
#pragma endregion

#pragma region Elastic
	float InElastic(float x);
	float OutElastic(float x);
	float InOutElastic(float x);
#pragma endregion

#pragma region Quart
	float InQuart(float x);
	float OutQuart(float x);
	float InOutQuart(float x);
#pragma endregion

#pragma region Expo
	float InExpo(float x);
	float OutExpo(float x);
	float InOutExpo(float x);
#pragma endregion

	/// <summary>
	/// イージングの種類
	/// </summary>
	enum class Type : int {
		Liner,
		InQuint,
		OutQuint,
		InOutQuint,
		InCirc,
		OutCirc,
		InOutCirc,
		InCubic,
		OutCubic,
		InOutCubic,
		InBack,
		OutBack,
		InOutBack,
		InBounce,
		OutBounce,
		InOutBounce,
		InElastic,
		OutElastic,
		InOutElastic,
		InQuart,
		OutQuart,
		InOutQuart,
		InExpo,
		OutExpo,
		InOutExpo,
		EasingCount	// 個数カウント用
	};
	/// <summary>
	/// イージングの種類名
	/// </summary>
	inline const char* kTypeNames[] = {
		"Liner",
		"InQuint",
		"OutQuint",
		"InOutQuint",
		"InCirc",
		"OutCirc",
		"InOutCirc",
		"InCubic",
		"OutCubic",
		"InOutCubic",
		"InBack",
		"OutBack",
		"InOutBack",
		"InBounce",
		"OutBounce",
		"InOutBounce",
		"InElastic",
		"OutElastic",
		"InOutElastic",
		"InQuart",
		"OutQuart",
		"InOutQuart",
		"InExpo",
		"OutExpo",
		"InOutExpo"
	};
	/// <summary>
	/// イージングの関数ポインタ配列
	/// </summary>
	inline std::function<float(float)> kFunction[] = {
		&Easing::Liner,
		&Easing::InQuint,
		&Easing::OutQuint,
		&Easing::InOutQuint,
		&Easing::InCirc,
		&Easing::OutCirc,
		&Easing::InOutCirc,
		&Easing::InCubic,
		&Easing::OutCubic,
		&Easing::InOutCubic,
		&Easing::InBack,
		&Easing::OutBack,
		&Easing::InOutBack,
		&Easing::InBounce,
		&Easing::OutBounce,
		&Easing::InOutBounce,
		&Easing::InElastic,
		&Easing::OutElastic,
		&Easing::InOutElastic,
		&Easing::InQuart,
		&Easing::OutQuart,
		&Easing::InOutQuart,
		&Easing::InExpo,
		&Easing::OutExpo,
		&Easing::InOutExpo,
	};

	/// <summary>
	/// 指定したタイプのイージングを行う関数
	/// </summary>
	/// <param name="type">種類指定</param>
	/// <param name="x">実数</param>
	/// <returns>イージングの値</returns>
	float CallFunction(Type type, float x);
	/// <summary>
	/// イージングの種類を選択するGUIを呼び出す関数
	/// </summary>
	/// <param name="type"></param>
	/// <param name="label"></param>
	/// <returns></returns>
	bool SelectTypeDebugGUI(Type* type, std::string label = "Easing Type");
};