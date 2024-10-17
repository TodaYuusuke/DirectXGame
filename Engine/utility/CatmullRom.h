#pragma once
#include "../math/vector/Vector3.h"
#include <string>
#include <vector>

namespace LWP::Utility {
	/// <summary>
	/// Catmull-Romスプライン曲線
	/// </summary>
	class CatmullRom {
	public: // ** パブリックなメンバ変数 ** //
		// 補間
		float t = 0.0f;
		// スプライン曲線制御点
		std::vector<LWP::Math::Vector3> controlPoints;


	public: // ** メンバ関数 ** //
		/// <summary>
		/// コンストラクタ
		/// </summary>
		CatmullRom() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~CatmullRom() = default;

		/// <summary>
		/// 現在の補間地点の座標を求める関数
		/// </summary>
		LWP::Math::Vector3 GetPosition();
		
		/// <summary>
		/// Debug用のImGui
		/// </summary>
		void DebugGUI();


	private: // ** プライベートなメンバ関数 ** //

		// 補間
		LWP::Math::Vector3 Interpolation(LWP::Math::Vector3 p0, LWP::Math::Vector3 p1, LWP::Math::Vector3 p2, LWP::Math::Vector3 p3, float calcT);
	};
}