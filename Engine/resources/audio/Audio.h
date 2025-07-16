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
		/// オーディオのデータを読み込む
		/// </summary>
		/// <param name="fileName">読み込むファイルの名前</param>
		void Load(const std::string& fileName);
		/// <summary>
		/// オーディオのデータを読み込む（resources/model/を短縮ver）
		/// </summary>
		/// <param name="filePath">読み込むファイルの名前</param>
		void LoadShortPath(const std::string& filePath);
		/// <summary>
		/// オーディオのデータを読み込む（exeからのパス指定）
		/// </summary>
		/// <param name="path">exeからのパス</param>
		void LoadFullPath(const std::string& path);
    
		/// <summary>
		/// 1回再生
		/// </summary>
		void Play();
		/// <summary>
		/// 1回再生（音量指定）
		/// </summary>
		/// <param name="volume">音量（0.0f ~ 1.0f）</param>
		void Play(const float& volume);
		/// <summary>
		/// 無限ループ再生
		/// </summary>
		void PlayLoop();
		/// <summary>
		/// 無限ループ再生（音量指定）
		/// </summary>
		/// <param name="volume">音量（0.0f ~ 1.0f）</param>
		void PlayLoop(const float& volume);
		/// <summary>
		/// 任意の回数ループ再生
		/// </summary>
		/// <param name="loopCount">何回ループするか（0 ... ループなし、255 ... 無限ループ）</param>
		void PlayAnyLoop(const UINT& loopCount);
		/// <summary>
		/// オーディオ一時停止
		/// </summary>
		void Pause();
		/// <summary>
		/// オーディオ停止から再生
		/// </summary>
		void Resume();
		/// <summary>
		/// オーディオ完全に停止
		/// </summary>
		void Stop();

		/// <summary>
		/// 音量を取得する
		/// </summary>
		float GetVolume();
		/// <summary>
		/// 音量を設定する
		/// </summary>
		void SetVolume(float value);

		/// <summary>
		/// 再生中かを返す関数
		/// </summary>
		bool GetIsPlaying();

		/// <summary>
		/// Debug用のGUI
		/// </summary>
		void DebugGUI();


	private: // ** メンバ変数 ** //
		// オーディオデータへのポインタ
		AudioData* ptr_ = nullptr;
		// オーディオソース
		IXAudio2SourceVoice* pSourceVoice_ = nullptr;

		// ボリューム
		float volume_ = 1.0f;
	};
}