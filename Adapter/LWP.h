//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//
//　　　　　　　　自作エンジン試作第２号くん　　　　　　　　//
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーー//

#pragma once

namespace LWP {
	namespace Base {
		#include "../Engine/base/WinApp.h"
		#include "../Engine/base/DirectXCommon.h"
	}

	namespace Primitive {
		#include "../Engine/3d/DrawingProcessor.h"
	}

	namespace Math {
		#include "../Engine/math/Vector2.h"
		#include "../Engine/math/Vector3.h"
		#include "../Engine/math/Vector4.h"
		#include "../Engine/math/Matrix4x4.h"
	}


	class Adapter
	{
	public: // メンバ関数

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



#pragma region 描画関数

		// 三角形描画関数
		//static void DrawTriangle(Math::Vector3 pos1, Vector3 pos2, Vector3 pos3, unsigned int color, DrawingProcessor::FillMode fillMode);

#pragma endregion


		//ーーーーーーーーーーーーー//
		//　　　　メンバ変数　　　　//
		//ーーーーーーーーーーーーー//
	private:

		// ウィンドウ
		static Base::WinApp* winApp_;
		// DirectX
		static Base::DirectXCommon* directXCommon_;
		// 描画システム
		static DrawingProcessor* drawSystem_;
	};
}