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
		/// ウィンドウ
		/// </summary>
		namespace Window {

			// ウィンドウ解像度（横幅）
			static const UINT kResolutionWidth = 1920;
			// ウィンドウ解像度（縦幅）
			static const UINT kResolutionHeight = 1080;

		}

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

			// 複数描画の最大数（メインの描画はカウントしない）
			static const UINT kMaxMultiWindowRendering = 50;

			/// <summary>
			/// インデックスの最大数
			/// </summary>
			static const UINT kMaxIndex = 655350;

			/// <summary>
			/// 頂点のResourceの最大数
			/// </summary>
			static const UINT kMaxVertex = 655350;

			/// <summary>
			/// トランスフォームのResourceの最大数
			/// </summary>
#if _DEBUG
			static const UINT kMaxMatrix = 1280;
#else
			static const UINT kMaxMatrix = 12800;
#endif
			/// <summary>
			/// マテリアルの最大数
			/// </summary>
			static const UINT kMaxMaterial = 12800;

			/// <summary>
			/// テクスチャの最大数
			/// </summary>
			static const UINT kMaxTexture = 128;
		};

		/// <summary>
		/// 影に関するパラメータ
		/// </summary>
		namespace Shadow {

			/// <summary>
			/// 平行光源
			/// </summary>
			namespace Direction {
				/// <summary>
				/// 平行光源のResourceの最大数（１つにつき1回のレンダリングが起きるので注意）
				/// </summary>
				static const UINT kMaxCount = 1;

				/// <summary>
				/// シャドウマッピングの解像度スケール
				/// <para>default = 5</para>
				/// <para>Range = 1 ~ ∞</para>
				/// <para>値が大きい .... 影のふちがより鮮明になるが、レンダリングが重くなる</para>
				/// <para>値が小さい .... 影のふちがギザギザしてしまうが、レンダリングが軽くなる</para>
				/// </summary>
				//static const float kResolutionScale = 1.0f;
			}

			/// <summary>
			/// 点光源
			/// </summary>
			namespace Point {
				/// <summary>
				/// 点光源のResourceの最大数（１つにつき1回のレンダリングが起きるので注意）
				/// </summary>
				static const UINT kMaxCount = 8;

				/// <summary>
				/// シャドウマッピングの解像度スケール
				/// <para>default = 5</para>
				/// <para>Range = 1 ~ ∞</para>
				/// <para>値が大きい .... 影のふちがより鮮明になるが、レンダリングが重くなる</para>
				/// <para>値が小さい .... 影のふちがギザギザしてしまうが、レンダリングが軽くなる</para>
				/// </summary>
				//static const float kResolutionScale = 1.0f;
			}

			/// <summary>
			/// シャドウマップの最大数
			/// </summary>
			static const UINT kMaxShadowMap = Direction::kMaxCount + (Point::kMaxCount * 6);

			/// <summary>
			/// シャドウマッピングの解像度スケール（現在は共通のスケールを使う）
			/// <para>default = 5</para>
			/// <para>Range = 1 ~ ∞</para>
			/// <para>値が大きい .... 影のふちがより鮮明になるが、レンダリングが重くなる</para>
			/// <para>値が小さい .... 影のふちがギザギザしてしまうが、レンダリングが軽くなる</para>
			/// </summary>
			static const float kResolutionScale = 1.0f;

			/// <summary>
			/// アンチエイリアスを有効にするか（未実装）
			/// <para>※ シェーダーのコードを書き換える必要があるので、自動シェーダー生成の実装時に合わせて実装</para>
			/// </summary>
			static const bool kEnableAntiAliasing = true;
		}


		/// <summary>
		/// ポストプロセスに関するパラメータ
		/// </summary>
		namespace PostProcess {

			/// <summary>
			/// 歪曲収差（LensDistortion）の最大回数
			/// <para>default = 5</para>
			/// <para>Range = 1 ~ ∞</para>
			/// </summary>
			static const int kMaxLensDistortion = 5;

		}
	};
};

// 短縮形
namespace lwpC {
	using namespace LWP::Config;
}