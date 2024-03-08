#pragma once
#include <stdint.h>
#include <object/collider/Mask.h>

/// --- 設定用の定数 --- ///

namespace LWP::Config {
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
		static const UINT kVsync = 0;

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
	/// <summary>
	/// 当たり判定に関するパラメータ
	/// </summary>
	namespace Collider {
		// 当たり判定マスク用の列挙子定義
		enum MaskLayer : uint32_t {
			Player = ColMask0,
			Enemy = ColMask1,
			Layer2 = ColMask2,
			Layer3 = ColMask3,
			Layer4 = ColMask4,
			Layer5 = ColMask5,
			Layer6 = ColMask6,
			Layer7 = ColMask7,
			Layer8 = ColMask8,
			Layer9 = ColMask9,
			Layer10 = ColMask10,
			Layer11 = ColMask11,
			Layer12 = ColMask12,
			Layer13 = ColMask13,
			Layer14 = ColMask14,
			Layer15 = ColMask15,
			ALL = ColMaskALL,
		};
	};
};

// 短縮形
namespace lwpC {
	using namespace LWP::Config;
}
// 当たり判定マスクを楽に呼び出せるように
using MaskLayer = LWP::Config::Collider::MaskLayer;