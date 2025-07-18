#pragma once
#include "IPolicy.h"

namespace LWP::Primitive {
	/// <summary>
	/// 通常の描画用の機能
	/// </summary>
	class NormalPolicy :
		public IPolicy {
	protected: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		NormalPolicy() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~NormalPolicy() = default;


		void InitImpl(Material* material) override;
		void UpdateImpl(Vertex* vertices, Material* material) override;
		void DebugGUIImpl() override {};
		Math::Vector2 GetFitSizeImpl(Material* material) override { return material->texture.t.GetSize(); }
		std::string GetNameImpl() override { return "Normal"; }
	};
}