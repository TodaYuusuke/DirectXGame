#pragma once
#include "../../IObject.h"

#include "math/vector/Vector3.h"
#include "utility/Color.h"
#include "base/directX/resource/data/ConstantBuffer.h"
#include "base/directX/resource/rendering/shadow/SM_Direction.h"
#include "base/directX/resource/rendering/DepthStencil.h"

// 平行光源の構造体
namespace LWP::Base {
	struct DirectionalLightStruct {
		Math::Matrix4x4 vp;			// ViewProjectionをこっちにも登録
		Math::Matrix4x4 projectionInverse;	// 深度値の修正用
		Math::Vector4 color;		// ライトの色
		Math::Vector3 direction;	// ライトの向き
		float intensity;			// 輝度
		float shadowIntensity;		// 影の濃さ
		float bias;
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
		// 輝度
		float intensity = 1.0f;
		// 影の濃さ
		float shadowIntensity = 0.5f;
		// 平行光源の距離
		float distance = 50.0f;

		float bias = 0.00015f;
		//float bias = 0.001f;
		float range = 128.0f;
		float nearZ = 0.0f;
		float farZ = 500.0f;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		DirectionLight();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~DirectionLight();

		// 初期化
		void Init() override;
		// 更新
		void Update() override;

		// ビュープロジェクションを返す関数
		Math::Matrix4x4 GetViewProjection() const;

		// 平行光源のシェーダー用のバッファーを返す関数
		const Base::ConstantBuffer<Base::DirectionalLightStruct>& GetLightBuffer() const { return lightBuffer_; }
		// ビュープロジェクションのViewを返す関数
		D3D12_GPU_VIRTUAL_ADDRESS GetMatrixBufferView() { return viewBuffer_.GetGPUView(); }
		// シャドウマップを書き込む用のリソースを返す関数
		Base::SM_Direction* GetShadowMap() { return &shadowMap_; }

		// デバッグ用GUI
		void DebugGUI() override;

	private: // ** プライベートなメンバ変数 ** //

		// 平行光源のバッファー
		Base::ConstantBuffer<Base::DirectionalLightStruct> lightBuffer_;

		// シャドウマッピング用のViewProjectionを送るバッファー
		Base::ConstantBuffer<Math::Matrix4x4> viewBuffer_;
		// シャドウマップ
		Base::SM_Direction shadowMap_;
	};
}