#pragma once
#include "Sphere.h"

// 前方宣言
namespace LWP::Object::Collider {
	class Capsule;
}

namespace LWP::Primitive {
	/// <summary>
	///	カプセル
	/// </summary>
	class Capsule final
		: public Sphere {
	public: // ** パブリックなメンバ変数 ** //
		// 始点
		LWP::Math::Vector3 start() const { return worldTF.GetWorldPosition(); }
		// 終点
		Utility::Observer<LWP::Math::Vector3> end = LWP::Math::Vector3{ 0.0f,0.0f, 0.0f };
		

	public: // ** 関数 ** //

		// 初期化を呼び出す
		Capsule() { Init(); }

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

		/// <summary>
		/// スフィアコライダーから描画用のスフィアを生成
		/// </summary>
		/// <param name="sphere"></param>
		//void CreateFromCapsuleCol(const LWP::Object::Collider::Capsule& capsule);


	private: // ** プライベートな関数 ** //

		/// <summary>
		/// ImGui
		/// </summary>
		void DerivedDebugGUI(const std::string& label = "Derived") override;

		// 始点が変わっているかを監視するための変数
		Utility::Observer<LWP::Object::TransformQuat*> obsTransform = &worldTF;

		/// <summary>
		/// パラメータが変わっているかを検証
		/// </summary>
		/// <returns></returns>
		bool GetChanged() override;


		// スフィアコライダーから描画用のスフィアを生成する関数だが、使用しないので隠蔽
		//using Sphere::CreateFromSphereCol;

	};
}