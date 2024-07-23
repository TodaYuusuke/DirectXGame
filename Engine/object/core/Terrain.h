#pragma once
#include "../IObject.h"

namespace LWP::Object {
	/// <summary>
	/// 地形クラス
	/// </summary>
	class Terrain final : public IObject {
	public: // ** メンバ関数 ** //

		// デフォルトコンストラクタ
		Terrain();

		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::RendererManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;


	private: // ** メンバ変数 ** //

		// ** 当たり判定用 ** //
		struct Triangle {
			Math::Vector3 pos[3];
			Math::Vector3 normal;
			int index;
		};

	};
}
