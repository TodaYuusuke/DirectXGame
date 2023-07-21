#pragma once
#include "WorldTransform.h"

namespace LWP::Object {
	class IObject : public WorldTransform {
	public: // パブリックな変数
		// アクティブ切り替え
		bool isActive;

		// ** 純粋仮想関数 ** //

		// 初期化
		virtual void Initialize() = 0;
		// 更新
		virtual void Update() = 0;
		// 描画
		virtual void Draw() final;
	
	private:
		IObject() = delete;
		~IObject() = delete;
	};
}