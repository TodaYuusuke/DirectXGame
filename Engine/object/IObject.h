#pragma once
#include "TransformQuat.h"

// 前方宣言
namespace LWP::Base {
	class RendererManager;
}

namespace LWP::Object {
	/// <summary>
	/// オブジェクトの基底クラス
	/// </summary>
	class IObject {
	public: // パブリックな変数

		// 固有名詞
		std::string name = "IObject";

		// アクティブ切り替え
		bool isActive = true;
		// ワールドトランスフォーム
		TransformQuat worldTF;

	public: // ** 純粋仮想関数 ** //

		// 初期化
		virtual void Initialize() = 0;
		// 更新
		virtual void Update(Base::RendererManager* manager) = 0;

		// デバッグ用GUI
		virtual void DebugGUI() = 0;


	public: // ** メンバ関数 ** //
		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		IObject();

		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~IObject();
	};
}

/// <summary>
/// IObjectを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class ObjectT>
concept IsIObject = std::is_base_of<LWP::Object::IObject, ObjectT>::value;