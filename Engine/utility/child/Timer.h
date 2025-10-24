#pragma once
#include "../IUtility.h"

namespace LWP::Utility {
	/// <summary>
	/// タイマークラス
	/// </summary>
	class Timer final : public IUtility {
	public: // ** パブリックなメンバ変数 ** //

		// 計測する時間（秒）
		float targetTime = 0.0f;
		// タイムスケールを有効にするか
		bool enableTimeScale = true;


	public: // ** メンバ関数 ** //

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		Timer();
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Timer() = default;

		/// <summary>
		/// 更新
		/// </summary>
		void Update() override;
		/// <summary>
		/// 形状を取得
		/// </summary>
		/// <returns></returns>
		Type GetType() override { return Type::Timer; }
		
		/// <summary>
		/// 計測開始（最初から）
		/// </summary>
		void Start();
		/// <summary>
		/// 計測停止
		/// </summary>
		void Stop();
		/// <summary>
		///	計測再開
		/// </summary>
		void Resume();

		/// <summary>
		/// タイマーが終了したらtrueを返す
		/// </summary>
		/// <returns></returns>
		bool GetEnd();

		/// <summary>
		/// デバッグ用GUI
		/// </summary>
		void DebugGUI() override;


	private: // ** メンバ変数 ** //

		// 計測開始フラグ
		bool isStart_ = false;
		// 現在の経過時間
		float duration_ = 0.0f;
	};
}
