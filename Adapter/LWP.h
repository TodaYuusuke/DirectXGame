//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第２号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
#pragma once
#include <functional>

#include "../Engine/base/WinApp.h"
#include "../Engine/base/DirectXCommon.h"

#include "../Engine/math/Math.h"

#include "../Engine/object/ObjectManager.h"
#include "../Engine/object/WorldTransform.h"

#include "../Engine/primitive/Primitive.h"

//#include "../Engine/scene/SceneController.h"

// LightWeightParticle
namespace LWP {
	namespace Base {
		class WinApp;
		class DirectXCommon;
	}
	namespace Math {
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4x4;
	}
	namespace Object {
		class Manager;
		class WorldTransform;
	}
	namespace Primitive {
		class Manager;
		class Triangle;
	}
	namespace Scene {
		class Manager;
	}

	template<class T>
	concept IsIPrimitive = std::is_base_of<IPrimitive, T>::value;

	template<class TPrimitive>
	requires IsIPrimitive<TPrimitive>

	class Engine {
	public:
		/// <summary>
		/// エンジン起動
		/// </summary>
		static void Run();

		/// <summary>
		/// 三角形のインスタンスを作成
		/// </summary>
		static std::function<TPrimitive>* CreatePrimitiveInstance() { return new std::function<TPrimitive>(primitiveController_); }

	private: // メンバ関数

		/// <summary>
		/// ライブラリの初期化
		/// </summary>
		/// <param name="title">ウィンドウタイトル</param>
		/// <param name="width">ウィンドウの横幅</param>
		/// <param name="height">ウィンドウの縦幅</param>
		static void Initialize(const char* title, int width, int height);
		/// <summary>
		/// ウィンドウからのイベントをチェックする関数
		/// </summary>
		/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
		static bool ProcessMessage();


		/// <summary>
		/// フレーム開始
		/// </summary>
		static void BeginFrame();
		/// <summary>
		/// フレーム終了
		/// </summary>
		static void EndFrame();


		/// <summary>
		/// ライブラリの終了
		/// </summary>
		static void Finalize();


	private: // メンバ変数
		// ウィンドウ
		static Base::WinApp* winApp_;
		// DirectX
		static Base::DirectXCommon* directXCommon_;
		// 描画システム
		static Primitive::Manager* primitiveController_;

		// シーンマネージャー
		static Scene::Manager* sceneController_;
	};
}