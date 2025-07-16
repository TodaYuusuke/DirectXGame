#include "Audio.h"

#include "component/Resource.h"

using namespace LWP;
using namespace LWP::Resource;

void Audio::Load(const std::string& path) {
	ptr_ = LoadAudio(path);
}
void Audio::LoadShortPath(const std::string& path) {
	ptr_ = LoadAudioLongPath("resources/audio/" + path);
}
void Audio::LoadFullPath(const std::string& path) {
	ptr_ = LoadAudioLongPath(path);
}

void Audio::Play() { PlayAnyLoop(0); }
void Audio::Play(const float& volume) {
	volume_ = volume;
	Play();
}
void Audio::PlayLoop() { PlayAnyLoop(255); }
void Audio::PlayLoop(const float& volume) {
	volume_ = volume;
	PlayLoop();
}
void Audio::PlayAnyLoop(const UINT& loopCount) {
	// まず再生中なら止める
	if (GetIsPlaying()) { Stop(); }

	ptr_->SubmitSource(loopCount, &pSourceVoice_);
	pSourceVoice_->SetVolume(volume_);	// 音量は再生後にここでセット

	// 再生
	HRESULT hr = pSourceVoice_->Start();
	assert(SUCCEEDED(hr));
}

void Audio::Pause() {
	// そもそも再生してないなら何もしない
	if (!pSourceVoice_) { return; }
	pSourceVoice_->Stop();
}
void Audio::Resume() {
	// そもそも再生してないなら何もしない
	if (!pSourceVoice_) { return; }
	pSourceVoice_->Start();
}
void Audio::Stop() {
	// そもそも再生してないなら止めない
	if (!pSourceVoice_) { return; }

	// 再生中でなければ止めない
	XAUDIO2_VOICE_STATE state{};
	pSourceVoice_->GetState(&state);
	if (state.SamplesPlayed != 0) {
		pSourceVoice_->DestroyVoice();
		pSourceVoice_ = nullptr;
		//pSourceVoice_->Stop();
	}
}

float Audio::GetVolume() {
	return volume_;
}
void Audio::SetVolume(float value) {
	volume_ = value;
	pSourceVoice_->SetVolume(volume_);
}

bool Audio::GetIsPlaying() {
	// そもそも再生してないなら再生してない
	if (!pSourceVoice_) { return false; }

	XAUDIO2_VOICE_STATE voiceState; 
	pSourceVoice_->GetState(&voiceState);
	return (voiceState.BuffersQueued > 0); //バッファがキューにあれば再生中とみなす
}

void Audio::DebugGUI() {
	ImGui::SliderFloat("volume", &volume_, 0.0f, 1.0f);
	if (ImGui::Button("Set Volume")) { SetVolume(volume_); }
	ImGui::Text("----- Functions -----");
	ImGui::Text("now Playing ... %s", GetIsPlaying() ? "yes" : "no");
	if (ImGui::Button("Play")) { Play(); }
	if (ImGui::Button("Play(Loop)")) { PlayLoop(); }
	if (ImGui::Button("Pause")) { Pause(); }
	if (ImGui::Button("Resume")) { Resume(); }
	if (ImGui::Button("Stop")) { Stop(); }
}