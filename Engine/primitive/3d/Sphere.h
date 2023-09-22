#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace LWP::Primitive {
	class Surface;

	/// <summary>
	/// 球
	/// </summary>
	class Sphere final
		: public IPrimitive {
	public:
		// ** 変数 ** //

		Vertex* vertices;	// 描画する頂点


		// ** プロパティ変数 ** //

	private: // 分割数
		uint32_t subdivision_ = 16;
	public: // アクセッサ
		void Subdivision(uint32_t value);
		uint32_t Subdivision() { return subdivision_; }

	private: // 半径
		float radius_;
	public: // アクセッサ
		void Radius(float value);			//
		float Radius() { return radius_; }	//


	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sphere(Manager*);

		/// <summary>
		/// 描画
		/// </summary>
		void Draw(FillMode fillmode, Resource::Texture* texture = nullptr) override;

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const { return (subdivision_ + 1) * (subdivision_ + 1); }
		/// <summary>
		/// インデックスの数を返す関数
		/// </summary>
		int GetIndexCount() const { return subdivision_ * (subdivision_ - 1) * 2 * 3; }
		

	private: // ** プライベートな変数 ** //

		uint32_t* indexes;	// インデックス


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
		/// 頂点を再計算する関数
		/// </summary>
		void CalcVertices();
	};
}