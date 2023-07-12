#pragma once
#include "IScene.h"

namespace LWP::Scene {
	class Controller final {
	public: // メンバ関数

		// 初期化
		void Initialize();
		// 更新
		void Update();
		// 描画
		void Draw();

	private: // メンバ変数
		// 現在のシーン
		IScene* currentScene_;
	};
}