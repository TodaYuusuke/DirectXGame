#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <string>

namespace LWP::Resource {
	class Audio final {
	public:
		// ** 共通の変数 ** //

		// コンストラクタ
		Audio(IXAudio2* xAudio, const std::string& filePath);
		// デストラクタ
		~Audio();


		/// <summary>
		/// 再生
		/// </summary>
		void Play();


	private: // ** プロパティ変数 ** //
		// CommandManager上でのインデックス
		int index_;
	public: // アクセッサ
		// CommandManager上でのインデックスを読みとる
		int GetIndex() const { return index_; }
		// 別のMaterialと同期する
		void SetIndex(int value) { index_ = value; }


	private: // ** メンバ変数 ** //
		// XAudio2のポインタ
		IXAudio2* xAudio_ = nullptr;

		// 波形フォーマット
		WAVEFORMATEX wfex_;
		// バッファの先頭アドレス
		BYTE* pBuffer_;
		// バッファのサイズ
		unsigned int bufferSize_;


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