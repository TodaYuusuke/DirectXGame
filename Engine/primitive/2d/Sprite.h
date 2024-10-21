#pragma once
#include "../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// 2Dテクスチャ
	/// </summary>
	class Sprite final
		: public IPrimitive {
	public: // ** パブリックなメンバ関数 ** //

		// サイズ
		LWP::Utility::Observer<LWP::Math::Vector2> size = LWP::Math::Vector2{ 200.0f,200.0f };
		// アンカーポイント
		LWP::Utility::Observer<LWP::Math::Vector2> anchorPoint = LWP::Math::Vector2{ 0.0f,0.0f };


	public: // ** 関数 ** //

		// 初期化を呼び出す
		Sprite() { 
			isUI = true;
			material.enableLighting = false;
			Init();
		}

		/// <summary>
		/// 更新処理
		/// </summary>
		void Update() override;

		/// <summary>
		/// 頂点を生成する関数
		/// </summary>
		void CreateVertices() override;

		/// <summary>
		/// 頂点数を返す関数
		/// </summary>
		int GetVertexCount() const { return 4; }

	private: // ** 派生クラス用の関数をオーバーライド ** //

		/// <summary>
		/// 独自のメンバ変数用にImGuiを用意
		/// </summary>
		/// <param name="label"></param>
		void DerivedDebugGUI(const std::string& label);

		/// <summary>
		/// 一部の値のみ見れればいいので元の関数をオーバーライド
		/// </summary>
		/// <returns></returns>
		bool GetChanged() override;
	};
}