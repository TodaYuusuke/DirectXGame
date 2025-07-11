#pragma once
#include "IPlane.h"

namespace LWP::Primitive {
	/// <summary>
	/// 連番テクスチャ用の機能クラス
	/// </summary>
	class ISequence
		: virtual public IPlane {
	public: // ** パブリックなメンバ変数 ** //

		// 連番テクスチャのindex
		int index = 0;


	public: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		ISequence();
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~ISequence() = default;


		/// <summary>
		/// 初期化（ユーザー呼び出し不要）
		/// </summary>
		void Init() override;
		/// <summary>
		/// 更新（ユーザー呼び出し不要）
		/// </summary>
		void Update() override;

		/// <summary>
		/// テクスチャと座標を一致させる関数
		/// </summary>
		/// <param name="fileName"></param>
		void FitToTexture();
		/// <summary>
		///	連番テクスチャの分割サイズを設定する関数
		/// </summary>
		void SetSplitSize(Math::Vector2 size);


	protected: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// 派生クラス用のImGui
		/// </summary>
		void ChildDebugGUI() override;


	private: // ** メンバ変数 ** //

		/// <summary>
		/// 連番テクスチャ1枚分のサイズ
		/// </summary>
		Math::Vector2 splitSize_ = { 100.0f,100.0f };
	};
}