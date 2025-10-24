#pragma once
#include <string>

namespace LWP::Utility {
	/// <summary>
	/// インスタンス管理が必要な便利系の基底クラス
	/// </summary>
	class IUtility {
	public: // ** パブリックなメンバ変数 ** //

		// 名前
		std::string name = "";

	public: // ** メンバ関数 ** //
		// 形状
		enum Type {
			Timer,
			Count
		};

		/// <summary>
		/// コンストラクタ
		/// </summary>
		IUtility();
		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~IUtility();

		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Init() {}
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update() {}

		/// <summary>
		/// Debug用のImGUI
		/// </summary>
		virtual void DebugGUI() {}
		/// <summary>
		/// 種類を取得
		/// </summary>
		/// <returns></returns>
		virtual Type GetType() = 0;

	};
}