#pragma once
#include <math/vector/Vector3.h>
#include <utility/Color.h>

#include "../../IObject.h"

namespace LWP::Object {
	/// <summary>
	/// 点光源オブジェクト
	/// </summary>
	class PointLight : public IObject {
	public:	// ** パブリックなメンバ変数 ** //
		
		// ライトのワールド座標
		Math::Vector3 position = { 0.0f,0.0f,0.0f };
		// ライトの色
		Utility::Color color = Utility::ColorPattern::WHITE;
		// 輝度
		float intensity = 1.0f;
		// ライトの届く最大距離
		float radius = 5.0f;
		// 減衰率
		float decay = 2.0f;


	public: // ** メンバ関数 ** //
		
		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::CommandManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;
	};
}