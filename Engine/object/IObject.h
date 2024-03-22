#pragma once
#include "WorldTransform.h"

// 前方宣言
namespace LWP::Base {
	class CommandManager;
}

namespace LWP::Object {
	class IObject {
	public: // パブリックな変数

		// 固有名詞
		std::string name = "IObject";

		// アクティブ切り替え
		bool isActive = true;
		// ワールドトランスフォーム
		WorldTransform transform;

		// ** 純粋仮想関数 ** //

		// 初期化
		virtual void Initialize() = 0;
		// 更新
		virtual void Update(Base::CommandManager* manager) = 0;

		// デバッグ用GUI
		virtual void DebugGUI() = 0;

		virtual ~IObject() = default;
	};
}

/// <summary>
/// IObjectを継承したクラスのみを選択できるテンプレート
/// </summary>
template<class ObjectT>
concept IsIObject = std::is_base_of<LWP::Object::IObject, ObjectT>::value;