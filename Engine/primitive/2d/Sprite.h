#pragma once
#include "../IPrimitive.h"

namespace LWP::Primitive {
	/// <summary>
	/// 2Dテクスチャ
	/// </summary>
	class Sprite
		: public IPrimitive {
	public: // ** パブリックなメンバ変数 ** //

		// サイズ
		Math::Vector2 size = LWP::Math::Vector2{ 200.0f,200.0f };
		// アンカーポイント
		Math::Vector2 anchorPoint = LWP::Math::Vector2{ 0.0f,0.0f };

		// 連番テクスチャの場合に使用するインデックス
		int index = -1;


	public: // ** 関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Sprite() { 
			// 初期化を呼び出す
			isUI = true;
			material.enableLighting = false;
			Init();
		}

		/// <summary>
		/// テクスチャを読み込む関数（resources/texture直下からのパス）
		/// </summary>
		/// <param name="fileName"></param>
		void LoadTexture(const std::string& fileName);
		/// <summary>
		/// 連番アニメーション用のスプライトの際に設定を行う
		/// </summary>
		/// <param name="splitSize">1枚のサイズ</param>
		void SetSplitSize(Math::Vector2 splitSize);

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

	protected: // ** 派生クラス用の関数をオーバーライド ** //

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

		/// <summary>
		/// 頂点の座標を生成する処理
		/// </summary>
		virtual void CreateVerticesPosition();
	};
}