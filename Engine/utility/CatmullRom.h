#pragma once
#include "../math/vector/Vector3.h"
#include <string>
#include <vector>

namespace LWP::Utility {
	/// <summary>
	/// Catmull-Romスプライン曲線
	/// </summary>
	class CatmullRom {
	public:
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
		/// 現在の補間地点の座標をTを指定して求める関数
		/// </summary>
		LWP::Math::Vector3 GetPosition(float specifiedT);
		
		/// <summary>
		/// ImGui（未実装）
		/// </summary>
		void DebugGUI(const std::string& label = "CatmullRom");


	public: // ** メンバ変数 ** //

		// スプライン曲線制御点
		std::vector<LWP::Math::Vector3> controlPoints;
		// 補間
		float t = 0.0f;


	private: // ** メンバ関数 ** //

		// Catmull-Romスプライン曲線補間
		LWP::Math::Vector3 Interpolation(LWP::Math::Vector3 p0, LWP::Math::Vector3 p1, LWP::Math::Vector3 p2, LWP::Math::Vector3 p3, float calcT);
		
		// 曲線描画（未実装）
		void Draw(size_t segmentCount);
	};
}