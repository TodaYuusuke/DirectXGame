#pragma once
#include "Sphere.h"

namespace LWP::Primitive {
	/// <summary>
	///	カプセル
	/// </summary>
	class Capsule final
		: public IPrimitive3D {
	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Capsule();
		/// <summary>
		/// タイプを返す
		/// </summary>
		Type GetType() { return Type::Capsule; }
		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;
		/// <summary>
		/// デバッグ用ImGui
		/// </summary>
		void DebugGUI() override;

		/// <summary>
		/// スフィアコライダーから描画用のスフィアを生成
		/// </summary>
		/// <param name="sphere"></param>
		void CreateFromCapsuleCol(const Math::Vector3& s, const Math::Vector3& e, const float& r);


	public: // ** パブリックなメンバ変数 ** //

		// 始点から終点へのオフセット点
		LWP::Math::Vector3 localOffset = { 0.0f,0.0f,1.0f };


	private: // ** メンバ変数 ** //
		
		// 始点と終点を示す球
		LWP::Primitive::Sphere startSphere;
		LWP::Primitive::Sphere endSphere;
	};
}