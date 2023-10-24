#include "Audio.h"

#include <fstream>
#include <cassert>

using namespace LWP::Resource;

Audio::Audio(IXAudio2* xAudio, const std::string& filePath) {
	HRESULT hr;
	xAudio_ = xAudio;
	LoadWAV(filePath);

	// 波形フォーマットを元にSourceVoiveの生成
	hr = xAudio_->CreateSourceVoice(&pSourceVoice, &wfex_);
	assert(SUCCEEDED(hr));

	// 再生する波形のデータの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = pBuffer_;
	buf.AudioBytes = bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopCount_;

	// 波形データの再生
	pSourceVoice->SubmitSourceBuffer(&buf);
}

Audio::~Audio() {
	// バッファのメモリを解法
	delete[] pBuffer_;

	pBuffer_ = 0;
	bufferSize_ = 0;
	wfex_ = {};
}


void Audio::Play() {
	pSourceVoice->Start();
}
void Audio::Stop() {
	pSourceVoice->Stop();
}

void Audio::LoadWAV(const std::string& filePath) {
	// ファイルを読み込む
	std::ifstream file;
	file.open(filePath.c_str(), std::ios_base::binary);
	assert(file.is_open());

#pragma region RIFFチャンク読み込み
	WAV::RiffHeader riff{};
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
#pragma endregion

#pragma region Formatチャンク読み込み
	WAV::FormatChunk format{};
	file.read((char*)&format, sizeof(WAV::ChunkHeader));
	// チャンクヘッダーの確認
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
#pragma endregion

#pragma region Dataチャンク読み込み
	WAV::ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	// 本物のdataチャンク読み込み
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部（波形データの読み込み）
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();
#pragma endregion

	// メンバ変数に代入
	wfex_ = format.fmt;
	pBuffer_ = reinterpret_cast<BYTE*>(pBuffer);
	bufferSize_ = data.size;
}