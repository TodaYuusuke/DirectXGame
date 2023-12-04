#pragma once
#include <stdint.h>

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// 設定用の定数
	/// </summary>
	namespace Config {

		/// <summary>
		/// フレームレート関連
		/// </summary>
		namespace FrameRate {

			/// <summary>
			/// 垂直同期の切り替えフラグ
			/// <para>default = 1</para>
			/// <para>Range = 0 ~ 4</para>
			/// <para>0 .... 新しいフレームがキューに入れられた場合、前に提示されたフレームの残り時間をキャンセルし、このフレームを廃棄します。</para>
			/// <para>1 ~ 4 .... 少なくともn個の垂直ブランクのプレゼンテーションを同期します</para>
			/// </summary>
			static const UINT kVsync = 1;

			/// <summary>
			/// 固定するフレームレート
			/// <para>default = 60</para>
			/// <para>Range = 0 ~ ∞</para>
			/// </summary>
			static const UINT kFixedFPS = 60;
		};

		/// <summary>
		/// レンダリング系パラメータ
		/// </summary>
		namespace Rendering {

			/// <summary>
			/// 頂点のResourceの最大数
			/// </summary>
			static const UINT kMaxVertex = 655350;

			/// <summary>
			/// カメラのResourceの最大数
			/// </summary>
			static const UINT kMaxCameraVP = 1 * 2;

			/// <summary>
			/// トランスフォームのResourceの最大数
			/// </summary>
			static const UINT kMaxMatrix = 12800;

			/// <summary>
			/// 平行光源のResourceの最大数（１つにつき1回のレンダリングが起きるので注意）
			/// </summary>
			static const UINT kMaxDirectionLight = 1;
			/// <summary>
			/// 点光源のResourceの最大数（１つにつき6回のレンダリングが起きるので注意）
			/// </summary>
			static const UINT kMaxPointLight = 8;

			/// <summary>
			/// シャドウマップの最大数
			/// </summary>
			static const UINT kMaxShadowMap = kMaxDirectionLight + (kMaxPointLight * 6);

			/// <summary>
			/// シャドウマッピングの解像度スケール
			/// <para>default = 5</para>
			/// <para>Range = 1 ~ ∞</para>
			/// <para>値が大きい .... 影のふちがより鮮明になるが、レンダリングが重くなる</para>
			/// <para>値が小さい .... 影のふちがギザギザしてしまうが、レンダリングが軽くなる</para>
			/// </summary>
			static const float kShadowMapResolutionScale = 1.0f;
		};
	};
};