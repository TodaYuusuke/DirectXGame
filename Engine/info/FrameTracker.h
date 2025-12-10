#pragma once
#include <chrono>

#include "utility/other/Singleton.h"

#include "base/directX/resource/data/ConstantBuffer.h"

namespace LWP::Information {
	/// <summary>
	/// FPSを管理するクラス
	/// </summary>
	class FrameTracker : public Utility::ISingleton<FrameTracker> {
		friend class Utility::ISingleton<FrameTracker>;	// ISingletonをフレンドクラスにしてコンストラクタを呼び出せるように
	private: // ** シングルトン化に必要な処理 ** //

		/// <summary>
		/// コンストラクタをプライベートに
		/// </summary>
		FrameTracker();

	public:	// ** メンバ関数 ** //

		// 初期化
		void Initialize();

		// 計測開始
		void Start();
		// 計測終了
		void End();

		// 今フレームのFPSを取得
		double GetFPS();
		// 処理を開始してから経過した時間を返す関数
		double GetElapsedTimeMS();
		double GetElapsedTimeS();
		double GetElapsedTimeM();
		double GetElapsedTimeH();

		// デルタタイムを返す関数
		double GetDeltaTime();
		double GetDefaultDeltaTime();
		// デルタタイムに掛ける係数をセットする関数
		void SetTimeScale(float value);

		// ImGuiを用いて情報表示
		void DebugGUI();
		// タブ無しのDebug表示
		void TablessDebugGUI();

		// バッファーのViewを取得
		D3D12_GPU_VIRTUAL_ADDRESS GetPreFrameBufferView() { return preFrame_.GetGPUView(); }


	private: // ** メンバ定数 ** //
		
		// 平均FPS計測用のデータ配列
		const static int kFrameDurationSize_ = 30;
		
		// 1/N秒ピッタリの時間
		std::chrono::microseconds kMinTime;
		// 1/N秒よりわずかに短い時間
		std::chrono::microseconds kMinCheckTime;


	private: // ** メンバ変数 ** //

		// プログラム実行開始時間
		std::chrono::steady_clock::time_point programStartTime_{};
		// フレーム開始時間
		std::chrono::steady_clock::time_point frameStartTime_{};
		// フレーム終了時間
		std::chrono::steady_clock::time_point frameEndTime_{};
		
		// 次にデータを入れるindex
		short frameDurationIndex_ = 0;
		// フレーム経過時間の配列
		double frameDurations_[kFrameDurationSize_];

		// 経過フレーム
		long elapsedFrame_ = 0;
		// 60フレームごとのFPS
		double frameRate = 0;

		// デルタタイムに掛ける係数
		float timeScale_ = 1.0f;

		// 1フレームの情報を格納したコンスタントバッファー
		struct PerFrame {
			float time;	// ゲームを起動してからの時間
			float deltaTime;	// 1フレームの経過時間
		};
		Base::ConstantBuffer<PerFrame> preFrame_;
	};
}