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
		/// オブジェクトのインスタンスを作成
		/// </summary>
		/// <typeparam name="TObject">オブジェクトの種類</typeparam>
		/// <returns>オブジェクトのインスタンス</returns>
		template <IsIObject TObject>
		TObject* CreateInstance() {
			return System::engine->objectManager_->CreateInstance<TObject>();
		}

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
			/// コライダーのインスタンスを作成
			/// </summary>
			/// <typeparam name="TObject">オブジェクトの種類</typeparam>
			/// <returns>オブジェクトのインスタンス</returns>
			template <IsICollider TCollider>
			TCollider* CreateInstance() {
				return System::engine->colliderManager_->CreateInstance<TCollider>();
			}
		};
	};
};