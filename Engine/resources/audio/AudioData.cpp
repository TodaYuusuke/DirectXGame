#include "AudioData.h"

#include <fstream>
#include <cassert>
#include "utility/MyUtility.h"

using namespace LWP;
using namespace LWP::Resource;

AudioData::AudioData(IXAudio2* xAudio, const std::string& filePath) {
	xAudio_ = xAudio;
	//HRESULT hr;
	//LoadWAV(filePath);
	
	// ソースリーダーを作成
	IMFSourceReader* pMFSourceReader = nullptr;
	MFCreateSourceReaderFromURL(Utility::ConvertString(filePath).c_str(), NULL, &pMFSourceReader);
	// メディアタイプの取得
	IMFMediaType* pMFMediaType = nullptr;
	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);	
	pMFSourceReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMFMediaType);
	pMFMediaType->Release();
	pMFMediaType = nullptr;
	pMFSourceReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMFMediaType);
	// オーディオデータ形式の作成
	WAVEFORMATEX* waveFormat{ nullptr };
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	// データの読み込み
	std::vector<BYTE> mediaData;
	while (true)
	{
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		pMFSourceReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		mediaData.resize(mediaData.size() + cbCurrentLength);
		memcpy(mediaData.data() + mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	BYTE* pBuffer = new BYTE[mediaData.size()];
	memcpy(pBuffer, mediaData.data(), mediaData.size());

	wfex = *waveFormat;
	pBuffer_ = pBuffer;
	bufferSize_ = static_cast<unsigned int>(mediaData.size());

	// Byte情報を保持
	//memcpy(pBuffer_, mediaData.data(), mediaData.size());

	// 解放
	pMFMediaType->Release();
	pMFSourceReader->Release();
}

AudioData::~AudioData() {
	//CoTaskMemFree(waveFormat);
	//delete[] pBuffer_;
	//pBuffer_ = 0;
}

void AudioData::Play(float volume, UINT loopCount) {
	HRESULT hr;

	// 波形フォーマットを元にSourceVoiceの生成
	hr = xAudio_->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f);
	assert(SUCCEEDED(hr));


	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = pBuffer_;
	buf.AudioBytes = bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = loopCount;

	// 波形データの再生
	pSourceVoice->SubmitSourceBuffer(&buf);
	// 音量セット
	pSourceVoice->SetVolume(volume);
	// 再生
	hr = pSourceVoice->Start();
}
void AudioData::Stop() {
	// そもそも再生してないなら止めない
	if (!pSourceVoice) { return; }

	// 再生中でなければ止めない
	XAUDIO2_VOICE_STATE state{};
	pSourceVoice->GetState(&state);
	if (state.SamplesPlayed != 0) {
		pSourceVoice->Stop();
	}
}

void AudioData::SetVolume(float v) {
	pSourceVoice->SetVolume(v);
}

/*
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
*/