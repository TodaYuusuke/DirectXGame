//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第２号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
#pragma once
#include <functional>

#include "../Engine/base/WinApp.h"
#include "../Engine/base/DirectXCommon.h"

#include "../Engine/math/Math.h"

#include "../Engine/object/ObjectManager.h"
#include "../Engine/object/ObjectList.h"

#include "../Engine/primitive/PrimitiveList.h"
#include "../Engine/primitive/IPrimitive.h"


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
		class Camera;
	}
	namespace Primitive {
		class Manager;
		class Triangle;
	}
	namespace Scene {
		class Manager;
	}

	template<class PrimitiveT>
	concept IsIPrimitive = std::is_base_of<Primitive::IPrimitive, PrimitiveT>::value;

	class Engine {
	public:
		/// <summary>
		/// エンジン起動
		/// ※ユーザ呼び出し禁止
		/// </summary>
		static void Run();

		/// <summary>
		/// 描画の計算に使うカメラのビュープロジェクションのポインタをセットする
		/// </summary>
		/// <param name="mainCamera">使用するカメラのポインタ</param>
		static void SetMainCameraMatrix(Object::Camera* mainCamera);


		/// <summary>
		/// オブジェクトのインスタンスを作成
		/// </summary>
		/// <typeparam name="TObject">オブジェクトの種類</typeparam>
		/// <returns>オブジェクトのインスタンス</returns>
		template <IsIObject TObject>
		static TObject* CreateObjectInstance() { return objectManager_->CreateObjectInstance<TObject>(); }

		/// <summary>
		/// 形のインスタンスを作成
		/// </summary>
		/// <typeparam name="TPrimitive">形の種類</typeparam>
		/// <returns>形のインスタンス</returns>
		template <IsIPrimitive TPrimitive>
		static TPrimitive* CreatePrimitiveInstance() { return new TPrimitive(primitiveManager_); }

		
		/// <summary>
		/// 現在の解像度を返す
		/// </summary>
		static int GetWindowWidth() { return winApp_->GetClientWidth(); }
		static int GetWindowHeight() { return winApp_->GetClientHeight(); }
		static float GetWindowWidthf() { return static_cast<float>(winApp_->GetClientWidth()); }
		static float GetWindowHeightf() { return static_cast<float>(winApp_->GetClientHeight()); }
		static Math::Vector2 GetWindow() { return { static_cast<float>(winApp_->GetClientWidth()), static_cast<float>(winApp_->GetClientHeight()) }; }

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
		// オブジェクト管理
		static Object::Manager* objectManager_;
		// 描画管理
		static Primitive::Manager* primitiveManager_;

		// シーンマネージャー
		static Scene::Manager* sceneManager_;
	};
}