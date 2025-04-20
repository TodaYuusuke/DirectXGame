#pragma once
#include "Sprite.h"

namespace LWP::Primitive {
	/// <summary>
	/// 平面
	/// </summary>
	class Billboard2D
		: public Sprite {
	public: // ** パブリックなメンバ変数 ** //

		// サイズ
		Math::Vector2 billboardSize = LWP::Math::Vector2{ 0.7f,0.7f };


	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Billboard2D();

	protected: // ** 派生クラス用の関数をオーバーライド ** //

		/// <summary>
		/// 独自のメンバ変数用にImGuiを用意
		/// </summary>
		/// <param name="label"></param>
		void DerivedDebugGUI(const std::string& label) override;
		/// <summary>
		/// 頂点の座標を生成する処理
		/// </summary>
		void CreateVerticesPosition() override;
	};
}