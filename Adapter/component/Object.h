#pragma once
#include "System.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// ゲームオブジェクト関連
	/// </summary>
	namespace Object {
		/// <summary>
		/// <para>オブジェクトのインスタンスをエンジンにセットする関数</para>
		/// <para>※ ユーザー呼び出し禁止</para>
		/// </summary>
		void SetPointer(IObject* ptr);
		/// <summary>
		/// <para>オブジェクトのインスタンスをエンジンから解放する関数</para>
		/// <para>※ ユーザー呼び出し禁止</para>
		/// </summary>
		void DeletePointer(IObject* ptr);

		/// <summary>
		/// カメラに複数画面描画用のレンダーテクスチャを作成する関数
		/// </summary>
		/// <param name="camera">テクスチャを付与するカメラ</param>
		/// <param name="width">レンダリング解像度（横幅）</param>
		/// <param name="height">レンダリング解像度（縦幅）</param>
		void CreateRenderTexture(LWP::Object::Camera* camera, const int width, const int height);

		/// <summary>
		/// 当たり判定関連
		/// </summary>
		namespace Collider {
			/// <summary>
			/// <para>コライダーのインスタンスをエンジンにセットする関数</para>
			/// <para>※ ユーザー呼び出し禁止</para>
			/// </summary>
			void SetPointer(ICollider* ptr);
			/// <summary>
			/// <para>コライダーのインスタンスをエンジンから解放する関数</para>
			/// <para>※ ユーザー呼び出し禁止</para>
			/// </summary>
			void DeletePointer(ICollider* ptr);
		};
	};
};