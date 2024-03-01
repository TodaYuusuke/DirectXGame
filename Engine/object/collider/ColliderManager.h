#pragma once
#include "cAABB.h"
//#include "cOBB.h"
//#include "cSphere.h"

namespace LWP::Object::Collider {
	/// <summary>
	/// 当たり判定用の管理クラス
	/// </summary>
	class Manager {
	public: // ** パブリックなメンバ関数 ** //
		// コンストラクタ
		Manager() = default;
		// デストラクタ
		~Manager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();

	private: // ** メンバ関数 ** //


	};
};