#pragma once
#include <string>

namespace LWP::Primitive {
	/// <summary>
	/// ビルボード3D用の基底クラス
	/// </summary>
	class IBillboard3D {
	public: // ** パブリックなメンバ変数 ** //

		// ロックする軸ごとのフラグ
		struct AxisLockFlags {
			bool x = false;
			bool y = false;
			bool z = false;
		}axisLockFlag;


	public: // ** メンバ関数 ** //
		// 名前を返す関数（ISpriteのnameにアクセスする用）
		virtual std::string& GetName() = 0;

	};
}