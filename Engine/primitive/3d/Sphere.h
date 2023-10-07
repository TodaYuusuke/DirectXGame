#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace LWP::Primitive {
	/// <summary>
	/// 球
	/// </summary>
	class Sphere final
		: public IPrimitive {
	private: // 分割数
		uint32_t subdivision_ = 16;
	public: // アクセッサ
		void Subdivision(uint32_t value);
		uint32_t Subdivision() { return subdivision_; }

	private: // 半径
		float radius_ = 1.0f;
	public: // アクセッサ
		void Radius(float value);
		float Radius() { return radius_; }


	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		using IPrimitive::IPrimitive;

		/// <summary>
		/// 頂点を生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		void CreateVertices() override;
		/// <summary>
		/// インデックスを生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		void CreateIndexes() override;

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const override;
		/// <summary>
		/// インデックスの数を返す関数
		/// </summary>
		int GetIndexCount() const override;
		

	private: // ** プライベートな関数 ** //

		/// <summary>
		/// 経度分割1つ分の角度を求める
		/// </summary>
		/// <returns></returns>
		float GetLonEvery() const { return (float)(2.0f * M_PI) / (float)subdivision_; }
		/// <summary>
		/// 緯度分割1つ分の角度を求める
		/// </summary>
		/// <returns></returns>
		float GetLatEvery() const { return (float)M_PI / (float)subdivision_; }

		/// <summary>
		/// ImGui
		/// </summary>
		void DerivedDebugGUI(const std::string& label = "Derived") override;
	};
}