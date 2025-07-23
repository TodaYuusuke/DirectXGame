#pragma once
#include "IPolicy.h"
#include "math/Rectangle.h"

namespace LWP::Primitive {
	/// <summary>
	/// 部分的切り抜き用の機能クラス
	/// </summary>
	class ClipPolicy :
		public IPolicy {
	public: // ** パブリックなメンバ変数 ** //

		// 切り抜く範囲
		Math::Rectangle2D clipRect;


	protected: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ClipPolicy() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~ClipPolicy() = default;


		void InitImpl(Material* material) override;
		void UpdateImpl(Vertex* vertices, Material* material) override;
		void DebugGUIImpl() override;
		Math::Vector2 GetFitSizeImpl([[maybe_unused]] Material* material) override { return clipRect.max - clipRect.min; }
		std::string GetNameImpl() override { return "Clip"; }
		void PostLoadTextureImpl(Material* material) override;
	};
}