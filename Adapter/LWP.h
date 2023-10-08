//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第３号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
#pragma once
#include <memory>

#include "../Engine/base/WinApp.h"
#include "../Engine/base/DirectXCommon.h"
#include "../Engine/base/CommandManager.h"
#include "../Engine/base/ImGuiManager.h"

#include "../Engine/input/InputManager.h"

#include "../Engine/object/ObjectManager.h"
#include "../Engine/primitive/PrimitiveManager.h"
#include "../Engine/resources/ResourceManager.h"
#include "../Engine/scene/SceneManager.h"

#include <dxgidebug.h>

/// <summary>
/// システム関連（※ユーザ呼び出し禁止）
/// </summary>
namespace LWP::System {
	class Engine final {
	public: // ** メンバ関数 ** //
		/// <summary>
		/// エンジン起動（※ユーザ呼び出し禁止）
		/// </summary>
		void Run();

	private: // ** プライベートなメンバ関数達 ** //
		/// <summary>
		/// ライブラリの初期化
		/// </summary>
		/// <param name="title">ウィンドウタイトル</param>
		/// <param name="width">ウィンドウの横幅</param>
		/// <param name="height">ウィンドウの縦幅</param>
		void Initialize(const char* title, int width, int height);
		/// <summary>
		/// ウィンドウからのイベントをチェックする関数
		/// </summary>
		/// <returns>true ... メッセージが来ていた場合、false ... メッセージが来ていない場合</returns>
		bool ProcessMessage();

		/// <summary>
		/// フレーム開始
		/// </summary>
		void BeginFrame();
		/// <summary>
		/// フレーム終了
		/// </summary>
		void EndFrame();

		/// <summary>
		/// ライブラリの終了
		/// </summary>
		void Finalize();
		

	public: // ** パブリックなメンバ変数たち ** //
		// メモリリークチェック用構造体
		struct D3DResourceLeakChecker {
			~D3DResourceLeakChecker() {
				// リソースリークチェック
				//Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
				//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
				//	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
				//	debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
				//	debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
				//}
			}
		};
		std::unique_ptr<D3DResourceLeakChecker> memoryLeakChecker_;

		// ウィンドウ
		std::unique_ptr<Base::WinApp> winApp_;
		// DirectX
		std::unique_ptr<Base::DirectXCommon> directXCommon_;
		// 描画管理
		std::unique_ptr<Base::CommandManager> commandManager_;
		// IｍGuiManager
		std::unique_ptr<Base::ImGuiManager> imGuiManager_;

		// InputManager
		std::unique_ptr<Input::Manager> inputManager_;

		// オブジェクト管理
		std::unique_ptr<Object::Manager> objectManager_;
		// 描画管理
		std::unique_ptr<Primitive::Manager> primitiveManager_;
		// リソース管理
		std::unique_ptr<Resource::Manager> resourceManager_;

		// シーンマネージャー
		std::unique_ptr<Scene::Manager> sceneManager_;
	};
}