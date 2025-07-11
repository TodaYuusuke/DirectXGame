#pragma once
#include "ISprite.h"
#include "math/Rectangle.h"

namespace LWP::Primitive {
	/// <summary>
	/// 部分的に切り抜いて描画するスプライト
	/// </summary>
	class ClipSprite final
		: public ISprite {
	public: // ** パブリックなメンバ変数 ** //

		// 切り抜く範囲
		Math::Rectangle2D clipRect;

	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ClipSprite();

		/// <summary>
		/// 初期化（ユーザー呼び出し不要）
		/// </summary>
		void Init() override;
		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		void Update() override;

		/// <summary>
		/// 描画するテクスチャを読み込む関数（resources/texture直下からのパス）
		/// </summary>
		/// <param name="fileName"></param>
		void LoadTexture(const std::string& fileName) override;
		/// <summary>
		/// clipRectとSpriteの座標を一致させる関数
		/// </summary>
		void FitToTexture() override;

	protected: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 派生クラス用のImGui
		/// </summary>
		void ChildDebugGUI() override;
	};
}