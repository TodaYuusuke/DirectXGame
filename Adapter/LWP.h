//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第３号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
#pragma once
#include <memory>

#include "../Engine/base/WinApp.h"
#include "../Engine/base/DirectXCommon.h"
#include "../Engine/base/ImGuiManager.h"

#include "../Engine/input/InputManager.h"

#include "../Engine/object/ObjectManager.h"
#include "../Engine/object/core/collision/CollisionManager.h"
#include "../Engine/primitive/PrimitiveManager.h"
#include "../Engine/resources/ResourceManager.h"
#include "../Engine/scene/SceneManager.h"

#include "../Engine/info/DebugCamera.h"
#include "../Engine/info/FrameTracker.h"
#include "../Engine/info/OutputLog.h"

#include <dxgidebug.h>

/// <summary>
/// システム関連（※ユーザ呼び出し禁止）
/// </summary>
namespace LWP::System {
	class Engine final {
	public: // ** メンバ関数 ** //

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Engine() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Engine() = default;

		/// <summary>
		/// エンジン起動（※ユーザ呼び出し禁止）
		/// </summary>
		void StartUp(std::wstring str);
		/// <summary>
		/// エンジン起動（※ユーザ呼び出し禁止）
		/// </summary>
		void Run(IScene* firstScene);

		/// <summary>
		/// シーン切り替え時の初期化
		/// </summary>
		void InitializeForScene();

	private: // ** プライベートなメンバ関数達 ** //
		/// <summary>
		/// ライブラリの初期化
		/// </summary>
		/// <param name="title">ウィンドウタイトル</param>
		/// <param name="width">ウィンドウの横幅</param>
		/// <param name="height">ウィンドウの縦幅</param>
		void Initialize(std::wstring title, int width, int height);
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
		/// ImGui表示
		/// </summary>
		void DebugGUI();

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
		Base::WinApp* winApp_ = nullptr;
		// DirectX
		Base::DirectXCommon* directXCommon_ = nullptr;
		// IｍGuiManager
		std::unique_ptr<Base::ImGuiManager> imGuiManager_;

		// InputManager
		Input::Manager* input_ = nullptr;

		// オブジェクト管理
		Object::Manager* object_ = nullptr;
		Object::CollisionManager* collision_ = nullptr;
		// 描画管理
		Primitive::Manager* primitive_ = nullptr;
		// リソース管理
		Resource::Manager* resource_ = nullptr;

		// シーンマネージャー
		std::unique_ptr<Scene::Manager> sceneManager_;

		// デバッグカメラ
		std::unique_ptr<LWP::Information::DebugCamera> debugCamera_;
		// デバッグ用タイマークラス
		LWP::Information::FrameTracker* frameTracker_ = nullptr;

		// DebugGUIの表示フラグ
		bool isDebugGUIVisible_ = true;
	};
}