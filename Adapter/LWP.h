//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第２号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
#pragma once
#include <functional>
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#include "../Engine/base/WinApp.h"
#include "../Engine/base/DirectXCommon.h"
#include "../Engine/base/ImGuiManager.h"

#include "../Engine/math/Math.h"

#include "../Engine/object/ObjectManager.h"
#include "../Engine/object/ObjectList.h"

#include "../Engine/primitive/PrimitiveList.h"
#include "../Engine/primitive/IPrimitive.h"

#include "../Engine/resources/ResourceManager.h"
#include "../Engine/resources/Material.h"
#include "../Engine/resources/texture/Texture.h"

// LightWeightParticle
namespace LWP {
	namespace Base {
		class WinApp;
		class DirectXCommon;
		class ImGuiManager;
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
	}
	namespace Resource {
		class Manager;
		class Material;
		class Texture;
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
		static TPrimitive* CreatePrimitiveInstance() {
			TPrimitive* instance = new TPrimitive(primitiveManager_.get());
			instance->CreateVertices();
			return instance;
		}

		/// <summary>
		/// 3Dモデルのインスタンスを作成
		/// </summary>
		/// <typeparam name="TPrimitive">形の種類</typeparam>
		/// <returns>形のインスタンス</returns>
		static Primitive::Model* CreateModelInstance(const std::string& filename) {
			Primitive::Model* instance = new Primitive::Model(primitiveManager_.get());
			instance->LoadFile(filename);
			return instance;
		}

		/// <summary>
		/// テクスチャのインスタンスを作成
		/// </summary>
		/// <param name="filePath">読み込むファイルパス</param>
		/// <returns>テクスチャのインスタンス</returns>
		static Resource::Texture* CreateTextureInstance(const std::string& filePath) { return new Resource::Texture(directXCommon_.get(), filePath); }
		
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
		// メモリリークチェック用構造体
		struct D3DResourceLeakChecker {
			~D3DResourceLeakChecker() {
				// リソースリークチェック
				//Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
				//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
				//	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				//	debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				//	debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
				//	debug->Release();
				//}
			}
		};
		static std::unique_ptr<D3DResourceLeakChecker> memoryLeakChecker_;

		// ウィンドウ
		static std::unique_ptr<Base::WinApp> winApp_;
		// DirectX
		static std::unique_ptr<Base::DirectXCommon> directXCommon_;
		// IｍGuiManager
		static std::unique_ptr<Base::ImGuiManager> imGuiManager_;

		// オブジェクト管理
		static std::unique_ptr<Object::Manager> objectManager_;
		// 描画管理
		static std::unique_ptr<Primitive::Manager> primitiveManager_;

		// シーンマネージャー
		static std::unique_ptr<Scene::Manager> sceneManager_;
	};
}