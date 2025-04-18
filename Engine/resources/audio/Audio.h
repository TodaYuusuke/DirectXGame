#pragma once
#include "AudioData.h"

namespace LWP::Resource {
	/// <summary>
	/// オーディオクラス
	/// <para>実体はリソースマネージャーが持っているのでこれはアクセス用のクラス</para>
	/// </summary>
	class Audio final {
	public: // ** メンバ関数 ** //

		// コンストラクタ
		Audio() = default;
		// デストラクタ
		~Audio() = default;

		/// <summary>
		/// ファイル読み込み
		/// </summary>
		/// <param name="path">resources/直下からのパス</param>
		void Load(const std::string& path);
		/// <summary>
		/// ファイル読み込み
		/// </summary>
		/// <param name="path">exeからのパス</param>
		void LoadShortPath(const std::string& path);
		/// <summary>
		/// ファイル読み込み
		/// </summary>
		/// <param name="path">exeからのパス</param>
		void LoadLongPath(const std::string& path);
		
		/// <summary>
		/// 再生
		/// </summary>
		/// <param name="loopCount">ループ回数（255で無限ループ）</param>
		void Play(const UINT& loopCount = 0);
		/// <summary>
		/// 再生
		/// </summary>
		/// <param name="volume">音量</param>
		/// <param name="loopCount">ループ回数（255で無限ループ）</param>
		void Play(const float& volume, const UINT& loopCount = 0);
		/// <summary>
		/// 停止
		/// </summary>
		void Stop();
		/// <summary>
		/// 一時停止
		/// </summary>
		//void Pause();

		/// <summary>
		/// 音量を取得する
		/// </summary>
		float GetVolume();
		/// <summary>
		/// 音量を設定する
		/// </summary>
		void SetVolume(float value);


	private: // ** メンバ変数 ** //
		// オーディオデータへのポインタ
		AudioData* ptr_ = nullptr;

		// ボリューム
		float volume_ = 1.0f;
	};
}