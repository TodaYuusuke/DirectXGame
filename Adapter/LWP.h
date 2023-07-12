//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第２号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
#pragma once
#include "../Engine/base/WinApp.h"
#include "../Engine/base/DirectXCommon.h"

#include "../Engine/3d/3d.h"

#include "../Engine/math/math.h"


// LightWeightParticle
namespace LWP {
	namespace Base {
		class WinApp;
		class DirectXCommon;
	}
	namespace Primitive {
		class Controller;
		class Triangle;
	}
	namespace Math {
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4x4;
	}
	namespace Scene {
		class Controller;
	}

	class Engine {
	public:
		/// <summary>
		/// エンジン起動
		/// </summary>
		static void Run();

		/// <summary>
		/// 三角形のインスタンスを作成
		/// </summary>
		static Primitive::Triangle* CreateTriangle() { return new Primitive::Triangle(primitiveController_); }

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
		static Primitive::Controller* primitiveController_;

		// シーンマネージャー
		static Scene::Controller* sceneController_;
	};
}