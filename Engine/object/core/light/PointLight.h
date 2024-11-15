#pragma once
#include "../../IObject.h"

#include "math/vector/Vector3.h"
#include "utility/Color.h"

#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/resource/rendering/shadow/SM_Point.h"

// 点光源の構造体
namespace LWP::Base {
	struct PointLightStruct {
		Math::Vector4 color;	// ライトの色
		Math::Vector3 position;	// ライトのワールド座標
		float intensity;		// 輝度
		float radius;			// ライトの届く最大距離
		float decay;			// 減衰率
	};
}

namespace LWP::Object {
	/// <summary>
	/// 点光源オブジェクト
	/// </summary>
	class PointLight : public IObject {
	public:	// ** パブリックなメンバ変数 ** //
		
		// ライトの色
		Utility::Color color = Utility::ColorPattern::WHITE;
		// 輝度
		float intensity = 1.0f;
		// ライトの届く最大距離
		float radius = 5.0f;
		// 減衰率
		float decay = 2.0f;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		PointLight();


		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::RendererManager* manager) override;

		// デバッグ用GUI
		void DebugGUI() override;


		// 構造体への暗黙変換
		operator Base::PointLightStruct() {
			Base::PointLightStruct result;
			Math::Vector4 c = color.GetVector4();
			result.color = { c.x,c.y,c.z };
			result.position = worldTF.GetWorldPosition();
			result.intensity = intensity;
			result.radius = radius;
			result.decay = decay;
			return result;
		}

	private: // ** プライベートなメンバ変数 ** //

		// ViewProjection
		std::array<Base::ConstantBuffer<Math::Matrix4x4>, 6> viewBuffers_;
		// シャドウマップ
		Base::SM_Point shadowMap_;
	};
}