#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <string>

namespace LWP::Resource {
	class Audio final {
	public: // ** パブリックなメンバ変数 ** //

		// ループ回数
		// 255で無限ループ
		UINT loopCount_ = 1;
		
	public: // ** メンバ関数 ** //

		// コンストラクタ
		Audio(IXAudio2* xAudio, const std::string& filePath);
		// デストラクタ
		~Audio();


		/// <summary>
		/// 再生
		/// </summary>
		void Play();
		/// <summary>
		/// 停止
		/// </summary>
		void Stop();
		/// <summary>
		/// 一時停止
		/// </summary>
		//void Pause();


		/// <summary>
		/// ループ回数を指定する 0ならば無限ループ
		/// </summary>
		void SetLoopCount(UINT value) { loopCount_ = value; }


	private: // ** メンバ変数 ** //
		// XAudio2のポインタ
		IXAudio2* xAudio_ = nullptr;

		// 波形フォーマット
		WAVEFORMATEX wfex_;
		// バッファの先頭アドレス
		BYTE* pBuffer_;
		// バッファのサイズ
		unsigned int bufferSize_;

		// オーディオソース
		IXAudio2SourceVoice* pSourceVoice = nullptr;


	private: // ** プライベートな関数 ** //

		struct WAV {
			// チャンクヘッダ
			struct ChunkHeader {
				char id[4];		// チャンク毎のID
				int32_t size;	// チャンクサイズ
			};
			// RIFFヘッダチャンク
			struct RiffHeader {
				ChunkHeader chunk;	// "RIFF"
				char type[4];		// "WAVE"
			};
			// Formatチャンク
			struct FormatChunk {
				ChunkHeader chunk;	// "fmt"
				WAVEFORMATEX fmt;	// 波形フォーマット
			};
		};

		/// <summary>
		/// waveファイルを読み込む
		/// </summary>
		void LoadWAV(const std::string& filePath);
	};
}