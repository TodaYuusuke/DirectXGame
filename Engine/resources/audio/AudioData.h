#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <string>

namespace LWP::Resource {
	class AudioData final {
	public: // ** メンバ関数 ** //

		AudioData() = default;
		// コンストラクタ
		AudioData(IXAudio2* xAudio, const std::string& filePath);
		// デストラクタ
		~AudioData();


		/// <summary>
		/// 再生
		/// </summary>
		void Play(float volume, UINT loopCount);
		/// <summary>
		/// 停止
		/// </summary>
		void Stop();
		/// <summary>
		/// 一時停止
		/// </summary>
		//void Pause();

		/// <summary>
		/// 音量セット
		/// </summary>
		/// <param name="v"></param>
		void SetVolume(float v);

	private: // ** メンバ変数 ** //
		// XAudio2のポインタ
		IXAudio2* xAudio_ = nullptr;

		// オーディオソース
		IXAudio2SourceVoice* pSourceVoice = nullptr;

		// Waveデータ
		WAVEFORMATEX wfex;
		// バッファの先頭アドレス
		BYTE* pBuffer_;
		// バッファのサイズ
		unsigned int bufferSize_;



	private: // ** プライベートな関数 ** //

		//struct WAV {
		//	// チャンクヘッダ
		//	struct ChunkHeader {
		//		char id[4];		// チャンク毎のID
		//		int32_t size;	// チャンクサイズ
		//	};
		//	// RIFFヘッダチャンク
		//	struct RiffHeader {
		//		ChunkHeader chunk;	// "RIFF"
		//		char type[4];		// "WAVE"
		//	};
		//	// Formatチャンク
		//	struct FormatChunk {
		//		ChunkHeader chunk;	// "fmt"
		//		WAVEFORMATEX fmt;	// 波形フォーマット
		//	};
		//};

		/// <summary>
		/// waveファイルを読み込む
		/// </summary>
		//void LoadWAV(const std::string& filePath);
	};
}