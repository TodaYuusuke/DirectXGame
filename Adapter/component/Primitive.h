#pragma once
#include "System.h"

/// <summary>
/// LightWeightParticleEngine
/// </summary>
namespace LWP {
	/// <summary>
	/// 形状関連
	/// </summary>
	namespace Primitive {
		/// <summary>
		/// 形状のインスタンスを作成
		/// </summary>
		/// <typeparam name="TPrimitive">形の種類</typeparam>
		/// <returns>形のインスタンス</returns>
		template <IsIPrimitive TPrimitive>
		static TPrimitive* CreateInstance();

		/// <summary>
		/// 描画時に使用するカメラをセットする関数
		/// </summary>
		/// <param name="camera">カメラのポインタ</param>
		static void SetMainCameraMatrix(Object::Camera* camera);
	};
};