#pragma once
#include "object/core/Camera.h"

namespace LWP::Information {
	/// <summary>
	/// Debug用のカメラを管理するクラス
	/// </summary>
	class DebugCamera final {
	public: // メンバ関数
		
		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		
		/// <summary>
		/// Debugカメラ起動
		/// </summary>
		void Enable(LWP::Object::Camera* ptr);
		/// <summary>
		/// Debugカメラ停止
		/// </summary>
		void Disable();

		/// <summary>
		/// DebugカメラのTransformを適応する関数
		/// </summary>
		void ApplyTransform();
		
		/// <summary>
		/// Debugカメラの使用フラグを返す関数
		/// </summary>
		bool GetActiveFlag() { return camera_.isActive; }
		/// <summary>
		/// Debugカメラのポインタを返す関数
		/// </summary>
		LWP::Object::Camera* GetCamera() { return &camera_; }

		/// <summary>
		/// Debug用のImGui
		/// </summary>
		void DebugGUI(LWP::Object::Camera* ptr);


	private: // ** メンバ変数 ** //
		// 起動時のカメラ
		LWP::Object::Camera* cameraPtr_ = nullptr;
		
		// Debugカメラ
		LWP::Object::Camera camera_;
		
		// カメラの移動速度
		float cameraSpeed_ = 20.0f;
		// カメラの回転速度
		float cameraRotateSpeed_ = 0.011f;


	private: // ** プライベートなメンバ関数 ** //

		/// <summary>
		/// カメラの移動処理
		/// </summary>
		void Move();
		/// <summary>
		/// カメラの回転処理
		/// </summary>
		void Rotate();
	};
}