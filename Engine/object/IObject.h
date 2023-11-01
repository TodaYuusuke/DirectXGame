#pragma once
#include "WorldTransform.h"

namespace LWP::Object {
	class IObject {
	public: // パブリックな変数
		// アクティブ切り替え
		bool isActive;
		// ワールドトランスフォーム
		WorldTransform transform;

		// ** 純粋仮想関数 ** //

		// 初期化
		virtual void Initialize() = 0;
		// 更新
		virtual void Update() = 0;

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