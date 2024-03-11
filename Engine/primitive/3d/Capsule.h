#pragma once
#include "Sphere.h"

template<class T>
class TEMP;

namespace LWP::Primitive {
	/// <summary>
	///	カプセル
	/// </summary>
	class Capsule final
		: public Sphere {
	public: // ** パブリックなメンバ変数 ** //
		// 終点
		Utility::Observer<LWP::Math::Vector3> endOffset = LWP::Math::Vector3{ 0.0f,0.0f, 0.0f };
		// 始点
		LWP::Math::Vector3 start() const { return transform.GetWorldPosition(); }

	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		using Sphere::Sphere;

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

		/// <summary>
		/// パラメータが変わっているかを検証
		/// </summary>
		/// <returns></returns>
		bool GetChanged() override;


		// スフィアコライダーから描画用のスフィアを生成する関数だが、使用しないので隠蔽
		using Sphere::CreateFromSphereCol;

	};
}