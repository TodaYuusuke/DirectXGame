#pragma once
#include "Sphere.h"
#include "Cylinder.h"

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
		/// 埋め立てかワイヤーフレームで描画するかを切り替える
		/// </summary>
		void ChangeFillMode() override;
		/// <summary>
		/// 全マテリアルのカラーを変更する
		/// </summary>
		/// <param name="flag"></param>
		void SetAllMaterialColor(const Utility::Color& color);
		/// <summary>
		/// 全マテリアルのenableLightingを切り替え
		/// </summary>
		void SetAllMaterialLighting(bool flag) override;

		/// <summary>
		/// スフィアコライダーから描画用のスフィアを生成
		/// </summary>
		/// <param name="start">ワールド座標</param>
		/// <param name="offset">ローカルのオフセット</param>
		/// <param name="radius">半径</param>
		void CreateFromCapsuleCol(const Math::Vector3& start, const Math::Vector3& offset, const float& radius);


	private: // ** メンバ変数 ** //
		
		// 円柱
		LWP::Primitive::Cylinder cylinder;
		// 終点を示す球
		LWP::Primitive::Sphere endSphere;
	};
}