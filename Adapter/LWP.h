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
	private:
		static const int* widthPtr;
		static const int* heightPtr;
	public: // 現在の解像度
		static const int* const kWindowWidth;
		static const int* const kWindowHeight;

	public:
		/// <summary>
		/// エンジン起動
		/// ※ユーザ呼び出し禁止
		/// </summary>
		static void Run();

		/// <summary>
		/// 描画の計算に使うカメラのポインタをセットする
		/// </summary>
		/// <param name="mainCamera">使用するカメラのポインタ</param>
		static void SetMainCamera(Object::Camera* mainCamera);


		/// <summary>
		/// オブジェクトのインスタンスを作成
		/// </summary>
		/// <typeparam name="TObject">オブジェクトの種類</typeparam>
		/// <returns>オブジェクトのインスタンス</returns>
		template <IsIObject TObject>
		static TObject* CreateObjectInstance() { return objectManager_->CreateObjectInstance<TObject>(); }

		/// <summary>
		/// 三角形のインスタンスを作成
		/// </summary>
		 
		/// <summary>
		/// 形のインスタンスを作成
		/// </summary>
		/// <typeparam name="TPrimitive">形の種類</typeparam>
		/// <returns>形のインスタンス</returns>
		template <IsIPrimitive TPrimitive>
		static TPrimitive* CreatePrimitiveInstance() { return new TPrimitive(primitiveManager_); }

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