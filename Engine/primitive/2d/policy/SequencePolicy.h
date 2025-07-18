#pragma once
#include "IPolicy.h"

namespace LWP::Primitive {
	/// <summary>
	/// 連番テクスチャ用の機能クラス
	/// </summary>
	class SequencePolicy :
		public IPolicy {
	public: // ** パブリックなメンバ変数 ** //

		// 連番テクスチャのindex
		int index = 0;

		/// <summary>
		///	連番テクスチャの分割サイズを設定する関数
		/// </summary>
		void SetSplitSize(Math::Vector2 size) { splitSize_ = size; }


	protected: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		SequencePolicy() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~SequencePolicy() = default;


		void InitImpl(Material* material) override;
		void UpdateImpl(Vertex* vertices, Material* material) override;
		void DebugGUIImpl() override;
		Math::Vector2 GetFitSizeImpl([[maybe_unused]] Material* material) override { return splitSize_; }
		std::string GetNameImpl() override { return "Sequence"; }


	private: // ** メンバ変数 ** //

		/// <summary>
		/// 連番テクスチャ1枚分のサイズ
		/// </summary>
		Math::Vector2 splitSize_ = { 100.0f,100.0f };
	};
}