#pragma once
#include "../../component/Vertex.h"
#include "../../component/Material.h"
#include <string>

namespace LWP::Primitive {
	/// <summary>
	/// 4頂点を操作しやすくするための列挙子
	/// </summary>
	enum QuadCorner : int {
		TopLeft = 0,		// 左上
		TopRight = 1,		// 右上
		BottomRight = 2,	// 右下
		BottomLeft = 3		// 左下
	};

	/// <summary>
	/// 各ポリシーの基底クラス
	/// </summary>
	class IPolicy {
	protected: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IPolicy() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IPolicy() = default;

		virtual void InitImpl(Material* material) = 0;
		virtual void UpdateImpl(Vertex* vertices, Material* material) = 0;
		virtual void DebugGUIImpl() = 0;
		virtual Math::Vector2 GetFitSizeImpl(Material* material) = 0;
		virtual std::string GetNameImpl() = 0;
		virtual void PostLoadTextureImpl([[maybe_unused]] Material* material) {}
	};
}