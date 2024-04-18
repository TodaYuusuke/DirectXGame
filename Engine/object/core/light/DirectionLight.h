#pragma once
#include "../../IObject.h"

#include "math/vector/Vector3.h"
#include "utility/Color.h"
#include "base/directX/utility/resource/data/ConstantBuffer.h"
#include "base/directX/utility/resource/rendering/shadow/SM_Direction.h"

// 平行光源の構造体
namespace LWP::Base {
	struct DirectionalLightStruct {
		Math::Matrix4x4 vp;		// ViewProjectionをこっちにも登録
		Math::Vector4 color;		// ライトの色
		Math::Vector3 direction;	// ライトの向き
		float intensity;	// 輝度
	};
}

namespace LWP::Object {
	/// <summary>
	/// 平行光源オブジェクト
	/// </summary>
	class DirectionLight : public IObject {
	public:	// ** パブリックなメンバ変数 ** //
		
		// ライトの色
		Utility::Color color = Utility::ColorPattern::WHITE;
		// ライトの向き
		Math::Vector3 rotation = { 1.57f,0.0f,0.0f };
		// 輝度
		float intensity = 1.0f;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		DirectionLight();
		
		// 初期化
		void Initialize() override;
		// 更新
		void Update(Base::RendererManager* manager) override;

		// ビュープロジェクションを返す関数
		Math::Matrix4x4 GetViewProjection() const;

		// デバッグ用GUI
		void DebugGUI() override;


		// 構造体への暗黙変換
		operator Base::DirectionalLightStruct() {
			Base::DirectionalLightStruct result;
			result.vp = GetViewProjection();
			result.color = color.GetVector4();
			result.direction = (Math::Vector3{ 0.0f,0.0f,1.0f } *Math::Matrix4x4::CreateRotateXYZMatrix(rotation)).Normalize();	// ライトの向き
			result.intensity = intensity;
			return result;
		}

	private: // ** プライベートなメンバ変数 ** //

		// ViewProjection
		Base::ConstantBuffer<Math::Matrix4x4> viewBuffer_;
		// シャドウマップ
		Base::SM_Direction shadowMap_;
	};
}