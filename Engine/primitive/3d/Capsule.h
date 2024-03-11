#pragma once
#include "Sphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace LWP::Primitive {
	/// <summary>
	///	カプセル
	/// </summary>
	class Capsule final
		: public Sphere {
	public: // ** 関数 ** //

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
		/// ImGui
		/// </summary>
		void DerivedDebugGUI(const std::string& label = "Derived") override;

		// スフィアコライダーから描画用のスフィアを生成する関数だが、使用しないので隠蔽
		using Sphere::CreateFromSphereCol;
	};
}