#pragma once
#include "../IPrimitive.h"

#define _USE_MATH_DEFINES
#include <math.h>

// 前方宣言
namespace LWP::Object::Collider {
	class Sphere;
}

namespace LWP::Primitive {
	/// <summary>
	/// 球
	/// </summary>
	class Sphere
		: public IPrimitive {
	public:	// ** パブリックなメンバ変数 ** //
		// 分割数
		Utility::Observer<uint32_t> subdivision = 16;
		// 半径
		Utility::Observer<float> radius = 1.0f;


	public: // ** 関数 ** //

		// 初期化を呼び出す
		Sphere() { Init(); }

		/// <summary>
		/// 頂点を生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		void CreateVertices() override;
		/// <summary>
		/// インデックスを生成する関数（ユーザ呼び出し禁止）
		/// </summary>
		void CreateIndexes() override;

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update() override;

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const override;
		/// <summary>
		/// インデックスの数を返す関数
		/// </summary>
		int GetIndexCount() const override;

		/// <summary>
		/// スフィアコライダーから描画用のスフィアを生成
		/// </summary>
		/// <param name="sphere"></param>
		//void CreateFromSphereCol(const LWP::Object::Collider::Sphere& sphere);


	protected: // ** プライベートな関数 ** //

		/// <summary>
		/// 経度分割1つ分の角度を求める
		/// </summary>
		/// <returns></returns>
		float GetLonEvery() const { return (float)(2.0f * M_PI) / static_cast<float>(subdivision.t); }
		/// <summary>
		/// 緯度分割1つ分の角度を求める
		/// </summary>
		/// <returns></returns>
		float GetLatEvery() const { return (float)M_PI / static_cast<float>(subdivision.t); }

		/// <summary>
		/// ImGui
		/// </summary>
		void DerivedDebugGUI(const std::string& label = "Derived") override;

		/// <summary>
		/// パラメータが変わっているかを検証
		/// </summary>
		/// <returns></returns>
		virtual bool GetChanged();
	};
}